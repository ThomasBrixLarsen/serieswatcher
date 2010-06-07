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

#ifndef EPISODE_MODEL_H
# define EPISODE_MODEL_H

#include <QtSql/QSqlQueryModel>

class TvDBCache;

class EpisodeModel : public QSqlQueryModel
{
  Q_OBJECT
public:
  enum Role { Type = Qt::UserRole, Id, ShowId, Season, Watched };

  EpisodeModel(TvDBCache *cache, QObject *parent = 0);


  Qt::ItemFlags flags(const QModelIndex & index) const;
  bool setData (const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
  QVariant data(const QModelIndex &item, int role) const;
  QVariant data(int row, int role, QVariant fallback = QVariant()) const;

public slots:
  void setSeason(int showId, int season);

private:
  TvDBCache *cache;
  int showId;
  int season;
};

#endif
