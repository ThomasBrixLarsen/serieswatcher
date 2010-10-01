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

#ifndef TREE_ITEM_H
#define TREE_ITEM_H

#include <QtCore/QList>
#include <QtCore/QVariant>

#include <QtTvDB>

class TvDBModel;

class TvDBItem
{
public:
  TvDBItem(int type, TvDBItem *parent = 0);
  ~TvDBItem();

  void appendChild(TvDBItem *child);

  TvDBItem *child(int row);
  int childCount() const;
  int columnCount() const;
  QVariant data(int column, int role) const;
  int row() const;
  Qt::ItemFlags flags() const;
  TvDBItem *parent();

  enum Type {
    Root,
    Show,
    Season,
    Episode
  };

private:
  QList< TvDBItem * > childItems;

  QtTvDB::Show *show;
  QtTvDB::Episode *episode;

  QString name;
  QString nextEpisodeName;
  QDateTime nextEpisodeDate;

  TvDBItem::Type itemType;
  int id;
  int episodes;
  int episodesWatched;
  int episodesNew;

  TvDBItem *parentItem;

  friend class TvDBModel;
};

#endif
