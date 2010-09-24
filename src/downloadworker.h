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

#ifndef DOWNLOAD_WORKER_H
# define DOWNLOAD_WORKER_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <QtTvDB>

#include "job.h"

class TvDBCache;

class DownloadWorker : public QObject
{
  Q_OBJECT
public:
  DownloadWorker(QObject *parent = 0);
  ~DownloadWorker();

public slots:
  void startJob(qint64 id, const QUrl & url, Job::Type type);
  void startJob(Job *job);
  void updateShow(qint64 id);
  void abort();

private slots:
  void downloadProgress(qint64 done, qint64 total);
  void downloadFinished(QNetworkReply *reply);
  void downloadError(QNetworkReply::NetworkError error);

signals:
  void newJob(Job *job);
  void downloadStarted(Job *job);
  void downloadFailed(Job *job, const QString & error);
  void downloadProgress(Job *job, qint64 done, qint64 total);
  void downloadFinished(Job *job);

  void error(const QString & title, const QString &message);
private:
  QtTvDB::Mirrors *mirrors;
  QNetworkAccessManager *manager;

  QMap < QNetworkReply *, Job * > downloads;
};

#endif
