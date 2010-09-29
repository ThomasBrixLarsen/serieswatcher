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
#include "tvdb.h"
#include "bannerloader.h"

ShowModel::ShowModel(TvDBCache *c, QObject *parent)
  : QSqlQueryModel(parent), cache(c)
{
  bannerLoader = new BannerLoader(this);
  connect(bannerLoader, SIGNAL(bannerReceived(int)), this, SLOT(bannerReceived(int)));

  refresh();
}

void
ShowModel::refresh()
{
  QString query;

  query = "SELECT shows.name, shows.id, COUNT(DISTINCT episodes.id) as episodesNb, "
    "COUNT(DISTINCT episodes.season) as seasons, "
    "SUM(episodes_extra.watched) as episodesWatched, "
    "MAX(CASE WHEN episodes_extra.watched THEN episodes.firstAired ELSE 0 END) as nextAirs "
    "FROM shows "
    "LEFT JOIN episodes ON shows.id = episodes.showId "
    "LEFT JOIN episodes_extra ON episodes.id = episodes_extra.id "
    "GROUP BY shows.id ";

  bannerLoader->clear();
  setQuery(query);
}

QVariant ShowModel::data(const QModelIndex &index, int role) const
{
  QVariant value = QSqlQueryModel::data(index, role);

  if (role >= Qt::UserRole || role == Qt::DecorationRole || role == Qt::DisplayRole)
    return data(index.row(), role, value);
  return value;
}

QVariant ShowModel::data(int row, int role, QVariant fallback) const
{
  QSqlRecord rec = record(row);

  if (role == ShowModel::Type)
    return QString("show");
  if (role == ShowModel::Id)
    return rec.value("id").toInt();
  if (role == ShowModel::Seasons)
    return rec.value("seasons").toInt();
  if (role == ShowModel::NextEpisode) {
    QDateTime date;
    date.setTime_t(rec.value("nextAirs").toInt());
    return date;
  }
  if (role == ShowModel::Episodes)
    return rec.value("episodesNb").toInt();
  if (role == ShowModel::EpisodesWatched)
    return rec.value("episodesWatched").toInt();
  if (role == ShowModel::EpisodesNotWatched)
    return rec.value("episodesNb").toInt() - rec.value("episodesWatched").toInt();
  if (role == Qt::DisplayRole)
    return rec.value("name").toString();
  if (role == Qt::DecorationRole)
      return fetchIcon(row, rec.value("id").toInt());
  return fallback;
}

QVariant
ShowModel::fetchIcon(int row, int id) const
{
  QtTvDB::Mirrors *mirrors = TvDB::mirrors();

  if (bannerLoader->hasBanner(row))
    return bannerLoader->banner(row);

  QString sql;
  sql = "SELECT banners.id as bannerId, banners.path FROM banners ";
  sql += "WHERE (banners.type = 'poster' AND banners.language = 'en') ";
  sql += QString("AND banners.showId = %1").arg(id);

  QSqlQuery query(sql);

  if (query.next())
    bannerLoader->fetchBanner(row, mirrors->bannerUrl(query.record().value("path").toString()));

  return QIcon::fromTheme("image-loading").pixmap(150);
}

void
ShowModel::bannerReceived(int row)
{
  QModelIndex idx = index(row, 0);
  emit dataChanged(idx, idx);
}
