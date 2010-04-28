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

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtGui/QPixmap>

#include "settings.h"
#include "tvdbcache.h"

#include "schema.h"

TvDBCache::TvDBCache()
{
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

  db.setDatabaseName(Settings::path() + QDir::separator() + "serieswatcher.db");
  db.open();
  QStringList queries = QString(schema).split(";");

  foreach(QString query, queries) {
    QSqlQuery q(query);

    if (!q.exec()) {
      qWarning() << q.lastError();
      qWarning() << query;
    }
  }
}

TvDBCache::~TvDBCache()
{
}

void
TvDBCache::storeShow(QtTvDB::Show *show)
{
}

void
TvDBCache::storeEpisode(QtTvDB::Episode *episode)
{
}

void
TvDBCache::storeEpisodes(QList < QtTvDB::Episode * > episodes)
{
}

void
TvDBCache::storeBanner(QtTvDB::Banner *banner)
{
}

void
TvDBCache::storeBanners(QList < QtTvDB::Banner * > banners)
{
}

QtTvDB::Show *
TvDBCache::fetchShow()
{
  return NULL;
}

QtTvDB::Episode *
TvDBCache::fetchEpisode()
{
  return NULL;
}

QList < QtTvDB::Episode * >
TvDBCache::fetchEpisodes(qint64 showId, qint64 seasonId)
{
  return QList < QtTvDB::Episode * >();
}

QtTvDB::Banner *
TvDBCache::fetchBanner(qint64 id)
{
  return NULL;
}

QList < QtTvDB::Banner * >
TvDBCache::fetchBanners(qint64 showId)
{
  return QList < QtTvDB::Banner * >();
}

QString
TvDBCache::bannerPath(qint64 id)
{
  QString path = Settings::path();
  QDir dir(path);

  if (!dir.exists("cache"))
    dir.mkdir("cache");
  dir.cd("cache");
  if (!dir.exists("banners"))
    dir.mkdir("banners");
  dir.cd("banners");
  return dir.filePath(QString("%1").arg(id));
}

void
TvDBCache::storeBannerFile(qint64 id, const QByteArray &data)
{
  QFile file(bannerPath(id));

  if (!file.open(QIODevice::WriteOnly))
    return;
  file.write(data);
  file.close();
}

bool
TvDBCache::hasBannerFile(qint64 id)
{
  return QFileInfo(bannerPath(id)).exists();
}

QPixmap
TvDBCache::fetchBannerFile(qint64 id)
{
  return QPixmap(bannerPath(id));
}
