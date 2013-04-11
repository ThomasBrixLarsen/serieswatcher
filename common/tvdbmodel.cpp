/*
 * Copyright (C) 2010-2011 Corentin Chary <corentin.chary@gmail.com>
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

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtCore/QUrl>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>
#include <QtGui/QPixmapCache>

#include "tvdbmodel.h"
#include "tvdb.h"
#include "tvdbitem.h"
#include "bannerloader.h"

TvDBModel::TvDBModel(QObject *parent)
  : QAbstractItemModel(parent)
{
  rootItem = new TvDBItem(TvDBItem::Root, NULL);
  bannerLoader = new BannerLoader(this);

  connect(bannerLoader, SIGNAL(bannerReceived(int)), this, SLOT(bannerReceived(int)));

  setupModelData();
}

TvDBModel::~TvDBModel()
{
  delete rootItem;
}

int
TvDBModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return static_cast<TvDBItem*>(parent.internalPointer())->columnCount();
  else
    return rootItem->columnCount();
}

bool
TvDBModel::setData(const QModelIndex & idx, const QVariant & data, int role)
{
  if (!idx.isValid())
    return false;

  TvDBItem *item = static_cast<TvDBItem*>(idx.internalPointer());

  if (item->setData(data, role)) {
    QModelIndex parent = idx.sibling(idx.row(), 0);

    while (parent.isValid()) {
      emit dataChanged(parent, parent.sibling(parent.row(), item->columnCount() - 1));
      parent = parent.parent();
    }

    return true;
  }
  return false;
}

QVariant
TvDBModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  TvDBItem *item = static_cast<TvDBItem*>(index.internalPointer());

  if (role == Qt::DecorationRole && index.column() == 7) {
    QVariant var;
    QString key = pixmapKey(index);
    QPixmap pm;

    if (bannerLoader->hasBanner((unsigned long)item)) { 
      pm = bannerLoader->banner((unsigned long)item).pixmap(200);
      QPixmapCache::insert(key, pm);
      return QIcon(pm);
    }
    if (QPixmapCache::find(key, &pm))
      return QIcon(pm);
    if (bannerRequests.contains((unsigned long)item))
      return QIcon::fromTheme("image-loading");

    var = item->data(index.column(), role);
    if (var.type() == QVariant::Url || var.type() == QVariant::String) {
      bannerRequests[(unsigned long)item] = index;
      bannerLoader->fetchBanner((unsigned long)item, var.toUrl());
    }

    return QIcon::fromTheme("image-loading");
  } else
    return item->data(index.column(), role);
}

void
TvDBModel::bannerReceived(int id)
{
  if (!bannerRequests.contains(id))
    return ;

  const QModelIndex item = bannerRequests[id];

  QPixmapCache::insert(pixmapKey(item), bannerLoader->banner(id).pixmap(200));
  emit dataChanged(item, item);
  bannerRequests.remove(id);
}

QString
TvDBModel::pixmapKey(const QModelIndex & index) const
{
  return QString("$tvdbmodel-%1-%2-%3")
    .arg(index.row())
    .arg(index.column())
    .arg((unsigned long)index.internalPointer());
}

Qt::ItemFlags
TvDBModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return 0;

  TvDBItem *item = static_cast<TvDBItem*>(index.internalPointer());

  return item->flags(index.column());
}

QVariant
TvDBModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return rootItem->data(section, role);

  return QVariant();
}

QModelIndex TvDBModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  TvDBItem *parentItem;

  if (!parent.isValid())
    parentItem = rootItem;
  else
    parentItem = static_cast<TvDBItem*>(parent.internalPointer());

  TvDBItem *childItem = parentItem->child(row);
  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

QModelIndex
TvDBModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();

  TvDBItem *childItem = static_cast<TvDBItem*>(index.internalPointer());
  TvDBItem *parentItem = childItem->parent();

  if (parentItem == rootItem)
    return QModelIndex();

  return createIndex(parentItem->row(), 0, parentItem);
}

int
TvDBModel::rowCount(const QModelIndex &parent) const
{
  TvDBItem *parentItem;
  if (parent.column() > 0)
    return 0;

  if (!parent.isValid())
    parentItem = rootItem;
  else
    parentItem = static_cast<TvDBItem*>(parent.internalPointer());

  return parentItem->childCount();
}

void
TvDBModel::reloadModel(void)
{
  bannerRequests.clear();
  setupModelData();
}

void
TvDBModel::setupModelData(void)
{
  TvDBItem *show, *season, *episode;
  QSqlQuery queryShow, querySeason, queryEpisode;

  QString sql;

  sql = "SELECT shows.name, shows.id FROM shows GROUP BY shows.id";

  queryShow.exec(sql);

  while (queryShow.next()) {
    show = new TvDBItem(TvDBItem::Show, rootItem);
    show->id = queryShow.record().value("id").toInt();
    show->name = queryShow.record().value("name").toString();

    sql = "SELECT episodes.season, episodes.seasonId FROM episodes ";
    sql += "WHERE episodes.showId = %1 ";
    sql += "GROUP BY episodes.season ";
    sql += "ORDER BY episodes.season ASC";
    sql = sql.arg(show->id);

    querySeason.exec(sql);

    while (querySeason.next()) {
      season = new TvDBItem(TvDBItem::Season, show);
      season->id = querySeason.record().value("seasonId").toInt();
      season->name = querySeason.record().value("season").toString();

      sql = "SELECT episodes.name, episodes.id, episodes_extra.watched, ";
      sql += "episodes.firstAired ";
      sql += "FROM episodes LEFT JOIN episodes_extra ";
      sql += "ON episodes.id = episodes_extra.id ";
      sql += "WHERE episodes.showId = %1 AND episodes.season = %2 ";
      sql += "ORDER BY episodes.episode ASC";
      sql = sql.arg(show->id).arg(season->name.toInt());

      queryEpisode.exec(sql);

      int episodeNumber = 1;
      while (queryEpisode.next()) {
	if(queryEpisode.record().value("name").toString().trimmed() == "" || queryEpisode.record().value("name").toString().trimmed() == "TBA")
		continue;
	episode = new TvDBItem(TvDBItem::Episode, show);
	episode->id = queryEpisode.record().value("id").toInt();
	episode->name = season->name+"x"+(episodeNumber>9? "":"0")+QString::number(episodeNumber)+" - "+queryEpisode.record().value("name").toString();
	episode->nextEpisodeName = episode->name;
	episode->nextEpisodeDate = QDateTime::fromTime_t(queryEpisode.record().value("firstAired").toULongLong());
	episode->episodesWatched = queryEpisode.record().value("watched").toBool();
	episode->episodes = 1;

	if (!episode->nextEpisodeDate.isNull() &&
	    episode->nextEpisodeDate < QDateTime::currentDateTime() &&
	    !episode->episodesWatched)
	  episode->episodesNew = true;

	season->appendChild(episode);
    episodeNumber++;
      }
      show->appendChild(season);
    }
    rootItem->appendChild(show);
  }
  rootItem->sortChildren();
}
