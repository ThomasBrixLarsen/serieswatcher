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

#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkDiskCache>
#include <QtGui/QDesktopServices>

#include "tvdb.h"
#include "bannerloader.h"
#include "downloadworker.h"

BannerLoader::BannerLoader(QObject *parent)
  : QObject(parent)
{
  DownloadWorker *worker = DownloadWorker::sharedInstance();

  diskCache = new QNetworkDiskCache(this);
  diskCache->setCacheDirectory(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));

  connect(&timer, SIGNAL(timeout()), this, SLOT(pendingTimeout()));
  connect(worker, SIGNAL(dataReceived(Job *, const QByteArray &)),
	  this, SLOT(jobFinished(Job *, const QByteArray &)));
}

BannerLoader::~BannerLoader()
{
  clear();
}

void
BannerLoader::clear()
{
  jobs.clear();
  replies.clear();
  banners.clear();
}

QIcon
BannerLoader::banner(int id)
{
  if (banners.contains(id))
    return banners[id];
  return QIcon();
}

bool
BannerLoader::hasBanner(int id)
{
  return banners.contains(id);
}

void
BannerLoader::fetchBanner(int id, const QUrl & url)
{
  DownloadWorker *worker = DownloadWorker::sharedInstance();
  QIODevice *device;
  Job *job;

  if (banners.contains(id) || replies.key(id) || jobs.key(id))
    return ;

  // FIXME: ask Bearer if network is up
  device = diskCache->data(url);
  if (device) {
    replies[device] = id;
    pending.append(device);
    timer.start(100);
  } else {
    job = worker->fetchBanner(id, url);
    jobs[job] = id;
  }
}

void
BannerLoader::pendingTimeout(void)
{
  foreach (QIODevice *device, pending) {
    if (replies.contains(device))
      bannerReceived(replies.take(device), device->readAll());
    device->deleteLater();
  }
  pending.clear();
}

void
BannerLoader::jobFinished(Job *job, const QByteArray & data)
{
  if (!jobs.contains(job))
    return ;

  bannerReceived(jobs.take(job), data);
}

void
BannerLoader::bannerReceived(int id, const QByteArray & data)
{
  QPixmap pixmap;

  if (banners.contains(id))
    return ;

  pixmap.loadFromData(data);
  banners[id] = pixmap;

  emit bannerReceived(id);
}
