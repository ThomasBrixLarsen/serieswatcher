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
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtGui/QPixmap>
#include <QtGui/QPixmapCache>
#include <QtGui/QApplication>
#include <QtCore/QThread>

#include "settings.h"
#include "tvdbcache.h"

#include "schema.h"

TvDBCache *TvDBCache::_instance = NULL;

TvDBCache *
TvDBCache::instance()
{
  if (QApplication::instance()->thread() != QThread::currentThread())
    return NULL;
  if (!_instance)
    _instance = new TvDBCache();
  return _instance;
}

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
TvDBCache::deleteShow(qint64 showId)
{
  QString sql;
  QSqlQuery query(db);

  sql = QString("DELETE FROM shows WHERE id = %1").arg(showId);

  query.prepare(sql);
  if (!query.exec()) {
    qWarning() << query.executedQuery();
    qWarning() << query.lastError();
  }

  sql = QString("DELETE FROM episodes WHERE showId = %1").arg(showId);

  query.prepare(sql);
  if (!query.exec()) {
    qWarning() << query.executedQuery();
    qWarning() << query.lastError();
  }

  sql = QString("DELETE FROM banners WHERE showId = %1").arg(showId);

  query.prepare(sql);
  if (!query.exec()) {
    qWarning() << query.executedQuery();
    qWarning() << query.lastError();
  }
}

void
TvDBCache::episodesWatched(qint64 showId, int season, bool watched)
{
  QString sql;
  QSqlQuery query(db);

  sql = "INSERT INTO episodes_extra ";
  sql += QString("SELECT id, %1 FROM episodes ").arg((int)watched);
  if (showId >= 0) {
    sql += QString("WHERE showId = %1 ").arg(showId);
    if (season >= 0)
      sql += QString("AND season = %1 ").arg(season);
  }

  query.prepare(sql);
  if (!query.exec()) {
    qWarning() << query.executedQuery();
    qWarning() << query.lastError();
  }
}

void
TvDBCache::episodeWatched(qint64 id, bool watched)
{
  QString sql;
  QSqlQuery query(db);

  sql = QString("INSERT INTO episodes_extra VALUES (%1, %2)").arg(id).arg((int)watched);

  query.prepare(sql);
  if (!query.exec()) {
    qWarning() << query.executedQuery();
    qWarning() << query.lastError();
  }
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
  query.addBindValue(episode->guestStars().join("|"));
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
  QSqlQuery query(QString("SELECT * FROM shows WHERE id = %1").arg(id), db);
  query.next();

  return showFromRecord(query.record());
}

QList < QtTvDB::Show * >
TvDBCache::fetchShows()
{
  QSqlQuery query("SELECT * FROM shows", db);
  QList < QtTvDB::Show * > list;

  while (query.next())
    list.append(showFromRecord(query.record()));
  return list;
}

QtTvDB::Episode *
TvDBCache::fetchEpisode(qint64 id)
{
  QSqlQuery query(QString("SELECT * FROM episodes WHERE id = %1").arg(id), db);
  query.next();

  return episodeFromRecord(query.record());
}

QList < QtTvDB::Episode * >
TvDBCache::fetchEpisodes(qint64 showId, qint64 seasonId)
{
  QSqlQuery query(QString("SELECT * FROM episodes WHERE showId = %1 AND season = %2").arg(showId).arg(seasonId), db);
  QList < QtTvDB::Episode * > list;

  while (query.next())
    list.append(episodeFromRecord(query.record()));
  return list;
}

QtTvDB::Banner *
TvDBCache::fetchBanner(qint64 id)
{
  QSqlQuery query(QString("SELECT * FROM banners WHERE id = %1").arg(id), db);
  query.next();

  return bannerFromRecord(query.record());
}

QList < QtTvDB::Banner * >
TvDBCache::fetchBanners(qint64 showId)
{
  QSqlQuery query(QString("SELECT * FROM banners WHERE showId = %1").arg(showId), db);
  QList < QtTvDB::Banner * > list;

  while (query.next())
    list.append(bannerFromRecord(query.record()));
  return list;
}

QtTvDB::Show *
TvDBCache::showFromRecord(QSqlRecord record)
{
  QtTvDB::Show *show = new QtTvDB::Show();

  show->setId(record.value("id").toULongLong());
  show->setName(record.value("name").toString());
  show->setOverview(record.value("overview").toString());
  show->setGenre(record.value("genre").toString().split("|"));
  show->setActors(record.value("actors").toString().split("|"));
  show->setNetwork(record.value("network").toString());
  show->setLanguage(record.value("language").toString());
  show->setContentRating(record.value("contentRating").toString());
  show->setRating(record.value("rating").toString());
  show->setStatus(record.value("status").toString());
  show->setRuntime(QTime::fromString(record.value("runtime").toString(), "m"));
  show->setAirsDay(record.value("airsDay").toString());
  show->setFirstAired(QDateTime::fromString(record.value("firstAired").toString()));
  show->setBanner(record.value("banner").toString());
  show->setPoster(record.value("poster").toString());
  show->setFanArt(record.value("fanart").toString());
  show->setImdbId(record.value("imdbId").toString());
  show->setSeriesId(record.value("seriesID").toString());
  show->setZap2ItId(record.value("zap2ItId").toString());
  show->setLastUpdated(QDateTime::fromTime_t(record.value("lastUpdated").toUInt()));

  return show;
}

QtTvDB::Episode *
TvDBCache::episodeFromRecord(QSqlRecord record)
{
  QtTvDB::Episode *episode = new QtTvDB::Episode();

  episode->setId(record.value("id").toULongLong());
  episode->setName(record.value("name").toString());
  episode->setShowId(record.value("showId").toULongLong());
  episode->setOverview(record.value("overview").toString());
  episode->setSeason(record.value("season").toUInt());
  episode->setEpisode(record.value("episode").toUInt());
  episode->setDirector(record.value("director").toString());
  episode->setGuestStars(record.value("guestStars").toString().split("|"));
  episode->setLanguage(record.value("language").toString());
  episode->setProductionCode(record.value("productionCode").toString());
  episode->setRating(record.value("rating").toString());
  episode->setWriter(record.value("writer").toString()); /* | */
  episode->setFirstAired(QDateTime::fromTime_t(record.value("firstAired").toUInt()));
  episode->setDvdChapter(record.value("dvdChapter").toString());
  episode->setDvdDiscId(record.value("dvdDiscid").toString());
  episode->setDvdEpisodeNumber(record.value("dvdEpisodeNumber").toString());
  episode->setDvdSeason(record.value("dvdSeason").toString());
  episode->setImage(record.value("image").toString());
  episode->setAirsAfterSeason(record.value("airsAfterAeason").toString());
  episode->setAirsBeforeSeason(record.value("airsBeforeSeason").toString());
  episode->setAirsBeforeEpisode(record.value("airsBeforeEpisode").toString());
  episode->setCombinedEpisode(record.value("combinedEpisode").toUInt());
  episode->setCombinedSeason(record.value("combinedSeason").toUInt());
  episode->setAbsoluteNumber(record.value("absoluteNumber").toUInt());
  episode->setSeasonId(record.value("seasonId").toUInt());
  episode->setEpImgFlag(record.value("epImgFlag").toString());
  episode->setImdbId(record.value("imdbId").toString());
  episode->setLastUpdated(QDateTime::fromTime_t(record.value("lastUpdated").toUInt()));

  return episode;
}

QtTvDB::Banner *
TvDBCache::bannerFromRecord(QSqlRecord record)
{
  QtTvDB::Banner *banner = new QtTvDB::Banner();

  banner->setId(record.value("id").toULongLong());
  banner->setPath(record.value("path").toString());
  banner->setType(record.value("type").toString());
  banner->setType2(record.value("type2").toString());
  banner->setLanguage(record.value("language").toString());
  banner->setSeason(record.value("season").toString());

  return banner;
}

void
TvDBCache::sync()
{
#ifndef Q_WS_WIN
  QFile file(db.databaseName());

  fsync(file.handle());
#endif
}

QString
TvDBCache::name()
{
  return dbName;
}
