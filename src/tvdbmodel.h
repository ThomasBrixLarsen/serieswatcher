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

#ifndef TVDB_MODEL_H
#define TVDB_MODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QModelIndex>
#include <QtCore/QVariant>

class TvDBItem;

class TvDBModel : public QAbstractItemModel
{
    Q_OBJECT

 public:
    TvDBModel(QObject *parent = 0);
    ~TvDBModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    enum Roles {
      Type = Qt::UserRole + 1,
      Id,
      Name,
      Episodes,
      EpisodesWatched,
      EpisodesNew,
      EpisodesNotWatched,
      NextEpisodeName,
      NextEpisodeDate,
      Banner
    };

    enum Type {
      Root,
      Show,
      Season,
      Episode
    };

 public slots:
    void reloadModel(void);

 private:
    void setupModelData(void);
    QString pixmapKey(const QModelIndex & index);

    TvDBItem *rootItem;
};

#endif
