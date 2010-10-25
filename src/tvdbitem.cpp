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
#include <QtGui/QIcon>
#include <QtGui/QApplication>
#include <QtGui/QStyle>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>

#include "tvdb.h"
#include "tvdbitem.h"
#include "tvdbmodel.h"
#include "tvdbcache.h"

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
  update(item);
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
  return 8;
}

void
TvDBItem::update(TvDBItem *item)
{
  episodes += item->episodes;
  episodesWatched += item->episodesWatched;
  episodesNew += item->episodesNew;

  if (item->episodesNew
      && (nextEpisodeName.isEmpty() || nextEpisodeDate > item->nextEpisodeDate)) {
    nextEpisodeName = item->nextEpisodeName;
    nextEpisodeDate = item->nextEpisodeDate;
  }
}

void
TvDBItem::recountTop()
{
  recount();
  if (parentItem)
    parentItem->recountTop();
}

void
TvDBItem::recountBottom()
{
  foreach (TvDBItem *item, childItems)
    item->recountBottom();
  recount();
}

void
TvDBItem::recount()
{
  episodes = 0;
  episodesNew = 0;

  if (itemType == TvDBItem::Episode) {
    episodes = 1;
    if (QDateTime::currentDateTime() > nextEpisodeDate && !episodesWatched)
      episodesNew = 1;
  } else {
    episodesWatched = 0;
    nextEpisodeName = QString();
    nextEpisodeDate = QDateTime();
  }

  foreach (TvDBItem *item, childItems)
    update(item);
}

void
TvDBItem::setWatched(bool watched)
{
  episodesWatched = watched;

  foreach (TvDBItem *item, childItems)
    item->setWatched(watched);
}

bool
TvDBItem::setData(const QVariant & value, int role)
{
  if (role == Qt::CheckStateRole || role == TvDBModel::EpisodesWatched) {
    TvDBCache *cache = TvDBCache::instance();
    bool watched = !!value.toInt();

    if (itemType == TvDBItem::Episode)
      cache->episodeWatched(id, watched);
    else if (itemType == TvDBItem::Season)
      cache->episodesWatched(parentItem->id, name.toInt(), watched);
    else if (itemType == TvDBItem::Show)
      cache->episodesWatched(id, -1, watched);
    else
      cache->episodesWatched(-1, -1, watched);

    setWatched(watched);
    recountBottom();
    recountTop();

    return true;
  }
  return false;
 }

QVariant
TvDBItem::data(int column, int role) const
{
  if (itemType == TvDBItem::Root && !parentItem) {
    QStringList columns;

    columns << QObject::tr("Name") << QObject::tr("Episodes")
	    << QObject::tr("Watched") << QObject::tr("New")
	    << QObject::tr("Not Watched") << QObject::tr("Next Episode")
	    << QObject::tr("Next Date") << QObject::tr("Banner");

    return columns[column];
  } else {
    if (itemType == TvDBItem::Episode && role == Qt::CheckStateRole
	&& (column == 0 || column == 7))
      return episodesWatched ? Qt::Checked : Qt::Unchecked;
    if (role == Qt::TextColorRole && episodesNew)
#if defined(Q_WS_MAEMO_5)
      return QColor(0x80, 0xB3, 0xFF);
#else
      return Qt::blue;
#endif
    if (column == 0) {
      if (role == Qt::DisplayRole) {
	if (itemType == TvDBItem::Season) {
	  if (name.toInt() == 0)
	    return QObject::tr("Specials");
	  else
	    return QObject::tr("Season %1").arg(name);
	} else
	  return name;
      }
      if (role == Qt::DecorationRole) {
	if (itemType == TvDBItem::Root)
	  return QApplication::style()->standardIcon(QStyle::SP_DirHomeIcon);
	if (itemType == TvDBItem::Show)
	  return QIcon::fromTheme("folder-video");
	if (itemType == TvDBItem::Season)
	  return QIcon::fromTheme("folder-video");
	if (itemType == TvDBItem::Episode)
	  return QIcon::fromTheme("video-x-generic");
	return QVariant();
      }
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

    if (column == 7) {
      /* FIXME */
      if (role == Qt::DisplayRole) {
	if (itemType == TvDBItem::Season) {
	  if (name.toInt() == 0)
	    return QObject::tr("Specials");
	  else
	    return QObject::tr("Season %1").arg(name);
	} else
	  return name;
      }
      if (role == Qt::DecorationRole) {
	QString sql;

	if (itemType == TvDBItem::Show) {
	  sql = "SELECT banners.path FROM banners ";
	  sql += "WHERE (banners.type = 'poster' AND banners.language = 'en') ";
	  sql += "AND banners.showId = %1";
	  sql = sql.arg(id);
	}
	if (itemType == TvDBItem::Season) {
	  sql = "SELECT banners.path FROM banners ";
	  sql += "WHERE banners.type = 'season' AND banners.type2 = 'season' ";
	  sql += "AND banners.season = %1 AND banners.language = 'en' ";
	  sql += "AND banners.showId = %2";
	  sql = sql.arg(name.toInt()).arg(parentItem->id);
	}
	if (itemType == TvDBItem::Episode) {
	  sql = "SELECT image as path FROM episodes WHERE episodes.id = %1";
	  sql = sql.arg(id);
	}

	QSqlQuery query(sql);

	if (query.next())
	  return TvDB::mirrors()->bannerUrl(query.record().value("path").toString());
	else if (query.lastError().number() != -1)
	  qWarning() << query.lastError();
	return QVariant();
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
TvDBItem::flags(int column) const
{
  Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (itemType == TvDBItem::Episode && (column == 0 || column == 7))
    flags |= Qt::ItemIsUserCheckable;
  return flags;
}
