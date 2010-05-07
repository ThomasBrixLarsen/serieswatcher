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
#include <QtSql/QSqlError>
#include <QtCore/QDateTime>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>

#include "tvdbcache.h"
#include "showmodel.h"

ShowModel::ShowModel(TvDBCache *c, QObject *parent)
  : QSqlQueryModel(parent), cache(c)
{
  QString query;

  /* next episode, number of season */
  query = "SELECT shows.name, shows.id, COUNT(DISTINCT episodes.id) as episodesNb, "
    "COUNT(DISTINCT episodes.season) as seasons, "
    "SUM(episodes_extra.watched) as episodesWatched, "
    "MAX(CASE WHEN episodes_extra.watched THEN episodes.firstAired ELSE 0 END) as nextAirs "
    "FROM shows "
    "LEFT JOIN episodes ON shows.id = episodes.showId "
    "LEFT JOIN episodes_extra ON episodes.id = episodes_extra.id "
    "GROUP BY shows.id ";

  setQuery(query);
}

QVariant ShowModel::data(const QModelIndex &index, int role) const
{
  if (role >= Qt::UserRole) {
    QSqlRecord rec = record(index.row());

    if (role == ShowModel::Id)
      return rec.value("id").toInt();
    if (role == ShowModel::Seasons)
      return rec.value("seasons").toInt();
    if (role == ShowModel::NextEpisode) {
      QDateTime date;
      qDebug() << rec.value("nextAirs");
      date.setTime_t(rec.value("nextAirs").toInt());
      return date;
    }
    if (role == ShowModel::Episodes)
      return rec.value("episodesNb").toInt();
    if (role == ShowModel::EpisodesNotWatched)
      return rec.value("episodesNb").toInt() - rec.value("episodesWatched").toInt();
  }

  QVariant value = QSqlQueryModel::data(index, role);

  if (role == Qt::DisplayRole)
    return value.toString();
  if (role == Qt::DecorationRole) {
    QString sql;

    sql = "SELECT banners.id as bannerId FROM banners ";
    sql += "WHERE (banners.type = 'poster' AND banners.language = 'en') ";
    sql += QString("AND banners.showId = %1").arg(record(index.row()).value("id").toInt());

    QSqlQuery query(sql);

    query.next();
    return QIcon(cache->fetchBannerFile(query.record().value("bannerId").toInt(), TvDBCache::Poster));
  }
  return value;
}

