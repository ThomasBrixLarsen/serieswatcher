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

#include "tvdbcache.h"
#include "showmodel.h"

ShowModel::ShowModel(TvDBCache *cache, QObject *parent)
  : QSqlQueryModel(parent)
{
  /* next episode, number of season */
  setQuery("SELECT shows.name, shows.id, "
	   "COUNT(DISTINCT episodes.season) as seasons FROM shows "
	   "JOIN episodes ON shows.id = episodes.showId GROUP BY shows.id");
}

QVariant ShowModel::data(const QModelIndex &index, int role) const
{
  if (role >= Qt::UserRole) {
    QSqlRecord rec = record(index.row());

    if (role == ShowModel::Id)
      return rec.value("id").toInt();
    if (role == ShowModel::Seasons)
      return rec.value("seasons").toInt();
    if (role == ShowModel::NextEpisode)
      return QDateTime();
  }

  QVariant value = QSqlQueryModel::data(index, role);

  if (role == Qt::DisplayRole)
    return value.toString();

  return value;
}

