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

#include <QtTvDB>

#include "updatethread.h"
#include "tvdb.h"

UpdateThread::UpdateThread(QObject *parent)
  : QThread(parent)
{
  manager = new QNetworkAccessManager(this);
  mirrors = TvDB::mirrors();
}

UpdateThread::~UpdateThread()
{
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
  startJob(id, mirrors->showAndEpisodesUrl(id), UpdateThread::ShowAndEpisodesXml);
  startJob(id, mirrors->bannersUrl(id), UpdateThread::BannersXml);
  //#endif
}

void
UpdateThread::startJob(qint64 id, const QUrl & url, UpdateThread::Type type)
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

  condition.wakeOne();
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
  forever {
    mutex.lock();
    working = false;

    if (jobs.isEmpty() && downloads.isEmpty())
      emit finished();

    while (jobs.isEmpty())
      condition.wait(&mutex);

    Job job = jobs.dequeue();

    working = true;
    mutex.unlock();

    QByteArray data = job.reply->readAll();

    if (job.type == UpdateThread::ShowAndEpisodesZip) {
    } else if (job.type == UpdateThread::Banner) {
    } else if (job.type == UpdateThread::BannersXml) {
      QList < QtTvDB::Banner * > banners = QtTvDB::Banner::parseBanners(data);

      /* Téléchaer */
      qDebug() << banners;
    } else if (job.type == UpdateThread::ShowAndEpisodesXml) {
      QList < QtTvDB::Show * > shows = QtTvDB::Show::parseShows(data);
      QList < QtTvDB::Episode * > episodes = QtTvDB::Episode::parseEpisodes(data);

      qDebug() << shows << episodes;
    }

    mutex.lock();
    job.reply->deleteLater();
    mutex.unlock();
  }
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
