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

#include <QtCore/QStringList>

#include "tvdbitem.h"
#include "tvdbmodel.h"

TvDBItem::TvDBItem(int type, TvDBItem *parent)
{
  parentItem = parent;
  itemType = (TvDBItem::Type) type;

  id = 0;
  episodes = 0;
  episodesWatched = 0;
  episodesNew = 0;
}

TvDBItem::~TvDBItem()
{
    qDeleteAll(childItems);
}

void
TvDBItem::appendChild(TvDBItem *item)
{
  item->parentItem = this;

  childItems.append(item);

  episodes += item->episodes;
  episodesWatched += item->episodesWatched;
  episodesNew += item->episodesNew;

  if (nextEpisodeName.isEmpty() && item->episodesNew) {
    nextEpisodeName = item->nextEpisodeName;
    nextEpisodeDate = item->nextEpisodeDate;
  }
}

TvDBItem *
TvDBItem::child(int row)
{
  return childItems.value(row);
}

int
TvDBItem::childCount() const
{
  return childItems.count();
}

int
TvDBItem::columnCount() const
{
  return 7;
}

QVariant
TvDBItem::data(int column, int role) const
{
  if (itemType == TvDBItem::Root) {
    QStringList columns;

    columns << QObject::tr("Name") << QObject::tr("Episodes") << ("Episodes Watched")
	    << QObject::tr("New Episodes") << QObject::tr("Episodes Not Watched")
	    << QObject::tr("Next Episode Name") << QObject::tr("Next Episode Date");

    return columns[column];
  } else {
    if (column == 0) {
      if (role == Qt::DisplayRole)
	return name;
      if (role == Qt::DecorationRole) {
	/* FIXME */
	return QVariant();
      }
      if (role == TvDBModel::Type)
	return (int) itemType;
      if (role == TvDBModel::Id)
	return id;
      if (role == TvDBModel::Name)
	return name;
      if (role > Qt::UserRole) {
	role = Qt::DisplayRole;
	column = role - Qt::DisplayRole + 3;
      }
    }
    if (role != Qt::DisplayRole)
      return QVariant();
    if (column == 1)
      return episodes;
    if (column == 2)
      return episodesWatched;
    if (column == 3)
      return episodesNew;
    if (column == 4)
      return episodes - episodesWatched;
    if (column == 5)
      return nextEpisodeName;
    if (column == 6)
      return nextEpisodeDate;
    return QVariant();
  }
}

TvDBItem *
TvDBItem::parent()
{
    return parentItem;
}

int
TvDBItem::row() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TvDBItem*>(this));

    return 0;
}

Qt::ItemFlags
TvDBItem::flags() const
{
  Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (itemType == TvDBItem::Episode)
    flags |= Qt::ItemIsUserCheckable;
  return flags;
}
