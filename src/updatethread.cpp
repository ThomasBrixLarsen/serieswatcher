/*
 * Copyright (C) 2010 Corentin Chary <corentin.chary@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <QtNetwork/QNetworkAccessManager>
#include <QtCore/QTimer>

#include <QtTvDB>

#include "updatethread.h"
#include "tvdb.h"
#include "tvdbcache.h"

UpdateThread::UpdateThread(QObject *parent)
  : QThread(parent), manager2(NULL)
{
  manager1 = new QNetworkAccessManager(this);
  manager2 = new QNetworkAccessManager();

  mirrors = TvDB::mirrors();
  cache = new TvDBCache();

  connect(manager1, SIGNAL(finished(QNetworkReply *)), this, SLOT(downloadFinished(QNetworkReply *)));
  connect(manager2, SIGNAL(finished(QNetworkReply *)), this, SLOT(downloadFinished(QNetworkReply *)));
}

UpdateThread::~UpdateThread()
{
  delete cache;
  delete manager2;
}

void
UpdateThread::update(qint64 id)
{
  QMutexLocker locker(&mutex);

  if (!isRunning())
    start(LowPriority);

  //#if HAVE_QUAZIP
  //  startJob(id, mirrors->showAndEpisodesUrlZip(id), UpdateThread::ShowAndEpisodesZip);
  //#else
  startJob(id, mirrors->showAndEpisodesUrl(id), UpdateThread::ShowAndEpisodesXml, manager1);
  startJob(id, mirrors->bannersUrl(id), UpdateThread::BannersXml, manager1);
  //#endif
}

void
UpdateThread::startJob(qint64 id, const QUrl & url, UpdateThread::Type type,
		       QNetworkAccessManager *manager)
{
  QNetworkReply *reply = manager->get(QNetworkRequest(url));

  connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
	  this, SLOT(downloadProgress(qint64, qint64)));
  connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
	  this, SLOT(downloadError(QNetworkReply::NetworkError)));

  Job job;

  job.id = id;
  job.type = type;
  job.state = UpdateThread::Downloading;
  job.url = url;
  job.reply = reply;

  if (jobs.isEmpty() && downloads.isEmpty())
    emit started();

  downloads[reply] = job;

  emit downloadStarted(id, job.type, url);
}

void
UpdateThread::downloadProgress(qint64 done, qint64 total)
{
  QNetworkReply *reply = dynamic_cast<QNetworkReply *>(sender());

  if (reply)
    emit downloadProgress(downloads[reply].id, done, total);
}

void
UpdateThread::downloadFinished(QNetworkReply *reply)
{
  emit downloadFinished(downloads[reply].id);

  mutex.lock();
  downloads[reply].state = UpdateThread::Parsing;

  jobs.enqueue(downloads[reply]);
  downloads.remove(reply);
  mutex.unlock();

  QTimer::singleShot(1, this, SLOT(processJobs()));
}

void
UpdateThread::downloadError(QNetworkReply::NetworkError error)
{
  QNetworkReply *r = dynamic_cast<QNetworkReply *>(sender());

  if (!r)
    return ;
  if (error == QNetworkReply::OperationCanceledError)
    return;

  emit downloadFailed(downloads[r].id, r->errorString());

  r->close(); /* Force finished() */
}

void
UpdateThread::run()
{
 exec();
}

void
UpdateThread::processJobs()
{
  mutex.lock();

  while (!jobs.isEmpty()) {
    working = false;

    Job job = jobs.dequeue();

    working = true;
    mutex.unlock();

    QByteArray data = job.reply->readAll();

    if (job.type == UpdateThread::ShowAndEpisodesZip) {
    } else if (job.type == UpdateThread::Banner) {
      cache->storeBannerFile(job.id, TvDBCache::Poster, data);
    } else if (job.type == UpdateThread::BannersXml) {
      QList < QtTvDB::Banner * > banners = QtTvDB::Banner::parseBanners(data);

      foreach ( QtTvDB::Banner * banner, banners) {
	cache->storeBanner(banner);

	if (banner->language() != "en")
	  continue ;
	if (banner->type() == "poster" || (banner->type() == "season" && banner->type2() == "season")) {
	  if (cache->hasBannerFile(banner->id(), TvDBCache::Poster))
	    continue ;
	  startJob(banner->id(), mirrors->bannerUrl(banner->path()), UpdateThread::Banner, manager2);
	}
      }
      qDeleteAll(banners);
    } else if (job.type == UpdateThread::ShowAndEpisodesXml) {
      QList < QtTvDB::Show * > shows = QtTvDB::Show::parseShows(data);
      QList < QtTvDB::Episode * > episodes = QtTvDB::Episode::parseEpisodes(data);

      foreach ( QtTvDB::Show * show, shows)
	cache->storeShow(show);

      foreach ( QtTvDB::Episode * episode, episodes)
	cache->storeEpisode(episode);

      qDeleteAll(shows);
      qDeleteAll(episodes);
    }

    mutex.lock();
    job.reply->deleteLater();
    mutex.unlock();
  }

  mutex.lock();
  if (jobs.isEmpty() && downloads.isEmpty())
    emit finished();
  mutex.unlock();
}

void
UpdateThread::abord()
{
  QMutexLocker locker(&mutex);

  foreach (Job job, downloads.values())
    job.reply->deleteLater();
  foreach (Job job, jobs)
    job.reply->deleteLater();
  jobs.clear();
  downloads.clear();
  terminate();

  emit finished();
}

bool
UpdateThread::updating()
{
  QMutexLocker locker(&mutex);

  return working || !downloads.isEmpty() || !jobs.isEmpty();
}
