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
}

void
EpisodeModel::setSeason(int show, int seas)
{
  QString query;

  showId = show;
  season = seas;

  query = "SELECT episodes.name, episodes.id, episodes_extra.watched, episodes.showId, episodes.season, ";
  query += " episodes.firstAired ";
  query += "FROM episodes ";
  query += "LEFT JOIN episodes_extra ";
  query += "ON episodes.id = episodes_extra.id ";
  query += QString("WHERE episodes.showId = %1 AND episodes.season = %2").arg(showId).arg(season);

  setQuery(query);
}

QVariant
EpisodeModel::data(const QModelIndex &index, int role) const
{
  QVariant value = QSqlQueryModel::data(index, role);

  if (role >= Qt::UserRole || role == Qt::DecorationRole || role == Qt::DisplayRole ||
      role == Qt::CheckStateRole || role == Qt::TextColorRole)
    return data(index.row(), role, value);
  return value;
}

Qt::ItemFlags
EpisodeModel::flags(const QModelIndex & index) const
{
  Qt::ItemFlags ret = QSqlQueryModel::flags(index);

  return ret | Qt::ItemIsUserCheckable;
}

bool
EpisodeModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid())
    return false;

  if (role == Qt::CheckStateRole) {
    QSqlRecord rec = record(index.row());
    bool val = false;

    if (value.toInt() == Qt::Checked)
      val = true;
    cache->episodeWatched(rec.value("id").toInt(), val);
    setSeason(showId, season); // Reload SQL data
    emit dataChanged(index, index);
    emit layoutChanged();
    return true;
  }
  return false;
}

QVariant
EpisodeModel::data(int row, int role, QVariant fallback) const
{
  QSqlRecord rec = record(row);

  if (role == Qt::CheckStateRole)
    return rec.value("watched").toBool() ? Qt::Checked : Qt::Unchecked;
  if (role == Qt::TextColorRole) {
    QDateTime airs = QDateTime::fromTime_t(rec.value("firstAired").toULongLong());

    if (!airs.isNull() && airs < QDateTime::currentDateTime() &&
	!rec.value("watched").toBool())
      return Qt::blue;
    return fallback;
  }
  if (role == EpisodeModel::Type)
    return QString("episode");
  if (role == EpisodeModel::Id)
    return rec.value("id").toInt();
  if (role == EpisodeModel::ShowId)
    return rec.value("showId").toInt();
  if (role == EpisodeModel::Season)
    return rec.value("season").toInt();
  if (role == EpisodeModel::Watched)
    return rec.value("watched").toInt();
  if (role == Qt::DisplayRole)
    return rec.value("name").toString();
  if (role == Qt::DecorationRole) {
    qint64 id = rec.value("id").toInt();

    return QIcon(cache->fetchBannerFile(id, TvDBCache::Episode, QSize(120, 150)));
  }
  return fallback;
}

