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

#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtCore/QDateTime>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>

#include "tvdbcache.h"
#include "seasonmodel.h"

SeasonModel::SeasonModel(TvDBCache *c, QObject *parent)
  : QSqlQueryModel(parent), cache(c)
{
  /* next episode, number of season */
}

void
SeasonModel::setShowId(int showId)
{
  QString query;

  query = "SELECT episodes.season, episodes.seasonId, COUNT(DISTINCT episodes.id) as episodes, ";
  query += "SUM(episodes_extra.watched) as episodesWatched, ";
  query += "episodes.showId as showId ";
  query += "FROM episodes ";
  query += "LEFT JOIN episodes_extra ON episodes.id = episodes_extra.id ";
  query += QString("WHERE episodes.showId = %1 ").arg(showId);
  query += "GROUP BY episodes.season";

  setQuery(query);
}

QVariant SeasonModel::data(const QModelIndex &index, int role) const
{
  if (role >= Qt::UserRole) {
    QSqlRecord rec = record(index.row());

    if (role == SeasonModel::Id)
      return rec.value("season").toInt();
    if (role == SeasonModel::ShowId)
      return rec.value("showId").toInt();
    if (role == SeasonModel::NextEpisode)
      return QDateTime();
    if (role == SeasonModel::Episodes)
      return rec.value("episodes").toInt();
    if (role == SeasonModel::EpisodesNotWatched)
      return rec.value("episodes").toInt();
  }

  QVariant value = QSqlQueryModel::data(index, role);

  if (role == Qt::DisplayRole) {
    int season = value.toInt();

    if (season)
      return tr("Season %1").arg(season);
    return tr("Specials");
  }
  if (role == Qt::DecorationRole) {
    QString sql;
    QSqlRecord rec = record(index.row());

    sql = "SELECT banners.id as bannerId FROM banners ";
    sql += "WHERE banners.type = 'season' AND banners.type2 = 'season' ";
    sql += "AND banners.season = %1 AND banners.language = 'en' ";
    sql += "AND banners.showId = %2";

    sql = sql
      .arg(rec.value("season").toInt())
      .arg(rec.value("showId").toInt());

    QSqlQuery query(sql);

    query.next();
    return QIcon(cache->fetchBannerFile(query.record().value("bannerId").toInt(), TvDBCache::Poster));
  }
  return value;
}

