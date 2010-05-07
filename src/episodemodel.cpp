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
#include <QtCore/QDateTime>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>

#include "tvdbcache.h"
#include "episodemodel.h"

EpisodeModel::EpisodeModel(TvDBCache *c, QObject *parent)
  : QSqlQueryModel(parent), cache(c)
{
  /* next episode, number of episode */
}

void
EpisodeModel::setSeason(int showId, int season)
{
  QString query;

  query = "SELECT episodes.name, episodes.id, episodes_extra.watched ";
  query += "FROM episodes ";
  query += "LEFT JOIN episodes_extra ";
  query += "ON episodes.id = episodes_extra.id ";
  query += QString("WHERE episodes.showId = %1 AND episodes.season = %2").arg(showId).arg(season);

  setQuery(query);
}

QVariant EpisodeModel::data(const QModelIndex &index, int role) const
{
  if (role >= Qt::UserRole) {
    QSqlRecord rec = record(index.row());

    if (role == EpisodeModel::Watched)
      return rec.value("watched").toInt();
  }

  QVariant value = QSqlQueryModel::data(index, role);

  if (role == Qt::DisplayRole)
    return value.toString();
  if (role == Qt::DecorationRole)
    return QIcon(cache->fetchBannerFile(record(index.row()).value("id").toInt(), TvDBCache::Episode));
  return value;
}

