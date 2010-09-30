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
#include "tvdb.h"
#include "bannerloader.h"

SeasonModel::SeasonModel(TvDBCache *c, QObject *parent)
  : QSqlQueryModel(parent), cache(c)
{
  bannerLoader = new BannerLoader(this);
  connect(bannerLoader, SIGNAL(bannerReceived(int)), this, SLOT(bannerReceived(int)));
  showId = -1;
}

void
SeasonModel::setShowId(int show)
{
  QString query;

  if (showId != show)
    bannerLoader->clear();

  showId = show;

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
  QVariant value = QSqlQueryModel::data(index, role);

  if (role >= Qt::UserRole || role == Qt::DecorationRole || role == Qt::DisplayRole)
    return data(index.row(), role, value);
  return value;
}

QVariant SeasonModel::data(int row, int role, QVariant fallback) const
{
  QSqlRecord rec = record(row);

  if (role == SeasonModel::Type)
    return QString("season");
  if (role == SeasonModel::Id)
    return rec.value("season").toInt();
  if (role == SeasonModel::ShowId)
    return rec.value("showId").toInt();
  if (role == SeasonModel::NextEpisode)
    return QDateTime(); /* FIXME */
  if (role == SeasonModel::Episodes)
    return rec.value("episodes").toInt();
  if (role == SeasonModel::EpisodesWatched)
    return rec.value("episodesWatched").toInt();
  if (role == SeasonModel::EpisodesNotWatched)
    return rec.value("episodes").toInt() - rec.value("episodesWatched").toInt();
  if (role == Qt::DisplayRole) {
    int season = rec.value("season").toInt();

    if (season)
      return tr("Season %1").arg(season);
    return tr("Specials");
  }
  if (role == Qt::DecorationRole)
    return fetchIcon(row, rec.value("showId").toInt(), rec.value("season").toInt());
  return fallback;
}

QVariant
SeasonModel::fetchIcon(int row, int showId, int season) const
{
  QtTvDB::Mirrors *mirrors = TvDB::mirrors();

  if (bannerLoader->hasBanner(row))
    return bannerLoader->banner(row);

  QString sql;

  sql = "SELECT banners.id as bannerId, banners.path FROM banners ";
  sql += "WHERE banners.type = 'season' AND banners.type2 = 'season' ";
  sql += "AND banners.season = %1 AND banners.language = 'en' ";
  sql += "AND banners.showId = %2";

  sql = sql.arg(season).arg(showId);

  QSqlQuery query(sql);

  if (query.next())
    bannerLoader->fetchBanner(row, mirrors->bannerUrl(query.record().value("path").toString()));

  return QIcon::fromTheme("image-loading").pixmap(150);
}

void
SeasonModel::bannerReceived(int row)
{
  QModelIndex idx = index(row, 0);
  emit dataChanged(idx, idx);
}
