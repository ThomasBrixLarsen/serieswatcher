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
#include <QtGui/QPixmapCache>

#include "settings.h"
#include "tvdbcache.h"

#include "schema.h"

TvDBCache::TvDBCache(const QString & name)
  : dbName(name)
{
  if (!QSqlDatabase::contains(name))
    connectDb(name);
}

TvDBCache::~TvDBCache()
{
}

bool
TvDBCache::connectDb(const QString & name)
{
  db = QSqlDatabase::addDatabase("QSQLITE", name);

  db.setDatabaseName(Settings::path() + QDir::separator() + "serieswatcher.db");

  if (!db.open())
    return false;

  QStringList queries = QString(schema).split(";");

  queries << "PRAGMA default_synchronous = 0;";
  queries << "PRAGMA synchronous = 0;";

  foreach(QString query, queries) {
    QSqlQuery q(query, db);

    if (!q.exec()) {
      qWarning() << q.lastError();
      return false;
    }
  }
  return true;
}

void
TvDBCache::storeShow(QtTvDB::Show *show)
{
  QString sql;
  QSqlQuery query(db);

  sql = "INSERT INTO shows VALUES ";
  sql += "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) ";

  query.prepare(sql);
  query.addBindValue(show->id());
  query.addBindValue(show->name());
  query.addBindValue(show->overview());
  query.addBindValue(show->genre().join("|"));
  query.addBindValue(show->actors().join("|"));
  query.addBindValue(show->network());
  query.addBindValue(show->contentRating());
  query.addBindValue(show->rating());
  query.addBindValue(show->runtime());
  query.addBindValue(show->status());
  query.addBindValue(show->language());
  query.addBindValue(show->firstAired().toTime_t());
  query.addBindValue(show->airsDay());
  query.addBindValue(show->airsTime());
  query.addBindValue(show->banner());
  query.addBindValue(show->poster());
  query.addBindValue(show->fanArt());
  query.addBindValue(show->imdbId());
  query.addBindValue(show->seriesId());
  query.addBindValue(show->zap2itId());
  query.addBindValue(show->lastUpdated().toTime_t());

  if (!query.exec()) {
    qWarning() << query.executedQuery();
    qWarning() << query.lastError();
  }
}

void
TvDBCache::storeShows(QList < QtTvDB::Show * > shows)
{
  foreach (QtTvDB::Show *show, shows)
    storeShow(show);
}

void
TvDBCache::storeEpisode(QtTvDB::Episode *episode)
{
  QString sql;
  QSqlQuery query(db);

  sql = "INSERT INTO episodes VALUES ";
  sql += "(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) ";

  query.prepare(sql);
  query.addBindValue(episode->id());
  query.addBindValue(episode->showId());
  query.addBindValue(episode->seasonId());
  query.addBindValue(episode->name());
  query.addBindValue(episode->overview());
  query.addBindValue(episode->season());
  query.addBindValue(episode->episode());
  query.addBindValue(episode->director());
  query.addBindValue(episode->guestStars());
  query.addBindValue(episode->language());
  query.addBindValue(episode->productionCode());
  query.addBindValue(episode->rating());
  query.addBindValue(episode->writer());
  query.addBindValue(episode->firstAired().toTime_t());
  query.addBindValue(episode->dvdChapter());
  query.addBindValue(episode->dvdDiscId());
  query.addBindValue(episode->dvdEpisodeNumber());
  query.addBindValue(episode->dvdSeason());
  query.addBindValue(episode->image());
  query.addBindValue(episode->airsAfterSeason());
  query.addBindValue(episode->airsBeforeSeason());
  query.addBindValue(episode->airsBeforeEpisode());
  query.addBindValue(episode->combinedEpisode());
  query.addBindValue(episode->combinedSeason());
  query.addBindValue(episode->absoluteNumber());
  query.addBindValue(episode->epImgFlag());
  query.addBindValue(episode->imdbId());
  query.addBindValue(episode->lastUpdated().toTime_t());

  if (!query.exec()) {
    qWarning() << query.executedQuery();
    qWarning() << query.lastError();
  }
}

void
TvDBCache::storeEpisodes(QList < QtTvDB::Episode * > episodes)
{
  foreach (QtTvDB::Episode *episode, episodes)
    storeEpisode(episode);
}

void
TvDBCache::storeBanner(QtTvDB::Banner *banner, qint64 showId)
{
  QString sql;
  QSqlQuery query(db);

  sql = "INSERT INTO banners VALUES ";
  sql += "(?, ?, ?, ?, ?, ?, ?) ";

  query.prepare(sql);
  query.addBindValue(banner->id());
  query.addBindValue(showId);
  query.addBindValue(banner->path());
  query.addBindValue(banner->type());
  query.addBindValue(banner->type2());
  query.addBindValue(banner->language());
  query.addBindValue(banner->season());

  if (!query.exec()) {
    qWarning() << query.executedQuery();
    qWarning() << query.lastError();
  }
}

void
TvDBCache::storeBanners(QList < QtTvDB::Banner * > banners, qint64 showId)
{
  foreach (QtTvDB::Banner *banner, banners)
    storeBanner(banner, showId);
}

QtTvDB::Show *
TvDBCache::fetchShow(qint64 id)
{
  QSqlQuery query("SELECT * FROM", db);

  return NULL;
}

QList < QtTvDB::Show * >
TvDBCache::fetchShows()
{
  return QList < QtTvDB::Show * >();
}

QtTvDB::Episode *
TvDBCache::fetchEpisode(qint64 id)
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
TvDBCache::bannerPath(qint64 id, BannerType type)
{
  QString path = Settings::path();
  QDir dir(path);
  QString subdir;

  if (!dir.exists("cache"))
    dir.mkdir("cache");
  dir.cd("cache");

  if (type == Poster)
    subdir = "poster";
  if (type == Banner)
    subdir = "banner";
  if (type == Search)
    subdir = "search";
  if (type == Episode)
    subdir = "episode";

  if (!dir.exists(subdir))
    dir.mkdir(subdir);
  dir.cd(subdir);
  return dir.filePath(QString("%1").arg(id));
}

void
TvDBCache::storeBannerFile(qint64 id, BannerType type, const QByteArray &data)
{
  QFile file(bannerPath(id, type));

  if (!file.open(QIODevice::WriteOnly))
    return;
  file.write(data);
  file.close();
}

bool
TvDBCache::hasBannerFile(qint64 id, BannerType type)
{
  return QFileInfo(bannerPath(id, type)).exists();
}

QPixmap
TvDBCache::fetchBannerFile(qint64 id, BannerType type)
{
  QPixmap pix;
  QString key = QString("tvdbcache-%1-%2").arg(id).arg(type);

  if (QPixmapCache::find(key, &pix))
    return pix;
  pix = QPixmap(bannerPath(id, type));
  QPixmapCache::insert(key, pix);
  return pix;
}

void
TvDBCache::sync()
{
  QFile file(db.databaseName());

//fsync(file.handle());
}

QString
TvDBCache::name()
{
  return dbName;
}
