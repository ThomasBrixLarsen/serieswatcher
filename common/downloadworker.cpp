/*
 * Copyright (C) 2010-2011 Corentin Chary <corentin.chary@gmail.com>
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

#include "config.h"

#include <QtGui/QDesktopServices>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkDiskCache>
#include <QtCore/QTimer>
#include <QtSql/QSqlDatabase>

#include <QtTvDB>

#include "downloadworker.h"
#include "tvdb.h"
#include "tvdbcache.h"

DownloadWorker *DownloadWorker::shared = NULL;

DownloadWorker::DownloadWorker(QObject *parent)
  : QObject(parent)
{
  QNetworkDiskCache *diskCache = new QNetworkDiskCache(this);

  diskCache->setCacheDirectory(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));

  manager = new QNetworkAccessManager(this);
  manager->setCache(diskCache);

  mirrors = TvDB::mirrors();

  connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(downloadFinished(QNetworkReply *)));
}

DownloadWorker::~DownloadWorker()
{

}

void
DownloadWorker::updateShow(qint64 id)
{
#ifdef WITH_ZIP
  startJob(id, mirrors->showAndEpisodesUrlZip(id, "en"), Job::ShowAndEpisodesZip);
#else
  startJob(id, mirrors->bannersUrl(id), Job::BannersXml);
  startJob(id, mirrors->showAndEpisodesUrl(id, "en"), Job::ShowAndEpisodesXml);
#endif
}

Job *
DownloadWorker::fetchBanner(int id, const QUrl & url)
{
  Job *job = startJob(id, url, Job::Banner);
  return job;
}

Job *
DownloadWorker::fetchEpisodeBanner(int id, const QUrl & url)
{
  Job *job = startJob(id, url, Job::EpisodeBanner);
  return job;
}

Job *
DownloadWorker::fetchSearchResults(const QString & query)
{
  Job *job = startJob(0, mirrors->searchShowUrl(query), Job::SearchResults);
  return job;
}

Job *
DownloadWorker::startJob(qint64 id, const QUrl & url, Job::Type type)
{
  Job *job = new Job();

  job->id = id;
  job->type = type;
  job->url = url;

  emit newJob(job);

  startJob(job);
  return job;
}

void
DownloadWorker::startJob(Job *job)
{
  QNetworkReply *reply;

  job->state = Job::Downloading;

  reply = manager->get(QNetworkRequest(job->url));
  connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
	  this, SLOT(downloadProgress(qint64, qint64)));
  connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
	  this, SLOT(downloadError(QNetworkReply::NetworkError)));

  downloads[reply] = job;

  emit downloadStarted(job);
}

void
DownloadWorker::downloadProgress(qint64 done, qint64 total)
{
  QNetworkReply *reply = dynamic_cast<QNetworkReply *>(sender());

  if (reply) {
    Job *job = downloads[reply];

    job->done = done;
    job->total = total;

    emit downloadProgress(job, done, total);
  }
}

void
DownloadWorker::downloadFinished(QNetworkReply *reply)
{
  QVariant location = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

  Job *job = downloads[reply];

  if (location.isValid()) {
    QUrl url = location.toUrl();

    if (url.isRelative()) {
      url.setScheme(reply->url().scheme());
      url.setEncodedHost(reply->url().encodedHost());
    }
    QNetworkRequest req(url);

    downloads.remove(reply);
    reply->deleteLater();

    job->url = url;

    reply = manager->get(QNetworkRequest(job->url));
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
	    this, SLOT(downloadProgress(qint64, qint64)));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
	    this, SLOT(downloadError(QNetworkReply::NetworkError)));

    downloads[reply] = job;
    return;
  }

  job->data = reply->peek(reply->bytesAvailable());
  job->state = Job::Waiting;

  downloads.remove(reply);
  reply->deleteLater();

  emit dataReceived(job, job->data);
  emit downloadFinished(job);
}

void
DownloadWorker::downloadError(QNetworkReply::NetworkError error)
{
  QNetworkReply *r = dynamic_cast<QNetworkReply *>(sender());

  if (!r)
    return ;
  if (error == QNetworkReply::OperationCanceledError)
    return;

  Job *job = downloads[r];

  job->state = Job::Waiting;

  emit downloadFailed(job, r->errorString());
}

void
DownloadWorker::abort()
{
  foreach (QNetworkReply *reply, downloads.keys())
    reply->deleteLater();

  downloads.clear();
}
