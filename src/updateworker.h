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

#ifndef UPDATE_WORKER_H
# define UPDATE_WORKER_H

#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QQueue>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

#include <QtTvDB>

#include "job.h"

class TvDBCache;

class UpdateWorker : public QObject
{
  Q_OBJECT
public:
  UpdateWorker(QObject *parent = 0);
  ~UpdateWorker();

  void abord();

public slots:
  void startJob(Job *job);

private:
  void parseShowAndEpisodesZip(Job *job);
  void parseBanner(Job *job);
  void parseBannersXml(Job *job);
  void parseShowAndEpisodesXml(Job *job);

signals:
  void newJob(Job *job);
  void parseStarted(Job *job);
  void parseProgress(Job *job, qint64 done, qint64 total);
  void parseFailed(Job *job);
  void parseFinished(Job *job);

  void error(const QString & title, const QString &message);

private:
  QMutex mutex;

  TvDBCache *cache;
  QtTvDB::Mirrors *mirrors;

  QQueue < Job * > jobs;
};

#endif
