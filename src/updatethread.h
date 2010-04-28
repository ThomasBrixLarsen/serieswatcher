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

#ifndef UPDATE_THREAD_H
# define UPDATE_THREAD_H

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QQueue>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <QtTvDB>

class TvDBCache;

class UpdateThread : public QThread
{
  Q_OBJECT
public:
  enum Type { ShowAndEpisodesXml, ShowAndEpisodesZip, BannersXml, Banner };
  enum State { Unknown, Downloading, Parsing, Finished };
  struct Job {
    qint64 id;
    Type type;
    State state;
    QUrl url;
    QNetworkReply *reply;
  };

  UpdateThread(QObject *parent = 0);
  ~UpdateThread();

  void run();
  void update(qint64 id);
  void abord();
  bool updating();

private:
  void startJob(qint64 id, const QUrl & url, UpdateThread::Type type,
		QNetworkAccessManager *manager);

private slots:
  void downloadProgress(qint64 done, qint64 total);
  void downloadFinished(QNetworkReply *reply);
  void downloadError(QNetworkReply::NetworkError error);
  void processJobs();

signals:
  void downloadStarted(qint64 id, UpdateThread::Type type, const QUrl &url);
  void downloadFailed(qint64 id, const QString & error);
  void downloadProgress(qint64 id, qint64 done, qint64 total);
  void downloadFinished(qint64 id);

  void parseStarted(qint64 id, const QString & name);
  void parseProgress(qint64 id, qint64 done, qint64 total);
  void parseFailed(qint64 id);
  void parseFinished(qint64 id);

  void databaseUpdated();
  void started();
  void finished();
  void error(const QString & title, const QString &message);
private:
  bool working;
  QMutex mutex;
  QWaitCondition condition;

  TvDBCache *cache;
  QtTvDB::Mirrors *mirrors;
  QNetworkAccessManager *manager1; /* For request in main thread */
  QNetworkAccessManager *manager2; /* For request in update thread (banners) */

  QQueue < Job > jobs;
  QMap < QNetworkReply *, Job > downloads;
};

#endif
