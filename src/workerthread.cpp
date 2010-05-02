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

#include "workerthread.h"
#include "updateworker.h"
#include "downloadworker.h"

WorkerThread::WorkerThread(QObject *parent)
  : QThread(parent)
{
  dworker = new DownloadWorker(); /* Download in main thread */
}

WorkerThread::~WorkerThread()
{
  delete dworker;
  delete uworker;
}

DownloadWorker *
WorkerThread::downloadWorker()
{
  return dworker;
}

UpdateWorker *
WorkerThread::updateWorker()
{
  return uworker;
}

void
WorkerThread::run()
{
  uworker = new UpdateWorker();
  uworker->moveToThread(this);

  connect(uworker, SIGNAL(newJob(Job *)), dworker, SLOT(startJob(Job *)));
  connect(dworker, SIGNAL(downloadFinished(Job *)), uworker, SLOT(startJob(Job *)));

  exec();
}

void
WorkerThread::abord()
{
  dworker->abord();
  uworker->abord();
}
