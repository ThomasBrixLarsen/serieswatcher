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

#ifndef BANNER_LOADER_H
# define BANNER_LOADER_H

#include <QtGui/QIcon>
#include <QtCore/QMap>
#include <QtCore/QUrl>
#include <QtCore/QTimer>

class QIODevice;
class QNetworkDiskCache;
struct Job;

class BannerLoader : public QObject
{
  Q_OBJECT
 public:
  BannerLoader(QObject *parent = NULL);
  ~BannerLoader();

  QIcon banner(int id);
  bool hasBanner(int id);
  void fetchBanner(int id, const QUrl & url);
  void clear();

 private slots:
  void pendingTimeout();
  void jobFinished(Job *job, const QByteArray & data);
  void bannerReceived(int id, const QByteArray & data);

 signals:
  void bannerReceived(int id);

 private:
  QNetworkDiskCache *diskCache;
  QMap < Job * , int > jobs;
  QMap < int, QIcon > banners;
  QMap < QIODevice *, int > replies;
  QList < QIODevice * > pending;
  QTimer timer;
};

#endif
