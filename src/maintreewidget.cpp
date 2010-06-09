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

#include <QtGui/QContextMenuEvent>

#include "maintreewidget.h"
#include "seriesaction.h"
#include "tvdbcache.h"
#include "episodemodel.h"
#include "showmodel.h"
#include "seasonmodel.h"

MainTreeWidget::MainTreeWidget(QWidget *parent)
  : QTreeWidget(parent)
{
  cache = new TvDBCache();
  menus = new SeriesMenus(this);

  connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem *)),
	  this, SLOT(treeItemCollapsed(QTreeWidgetItem *)));
  connect(this, SIGNAL(itemExpanded(QTreeWidgetItem *)),
	  this, SLOT(treeItemExpanded(QTreeWidgetItem *)));
}

MainTreeWidget::~MainTreeWidget()
{
  delete cache;
  delete menus;
}

void
MainTreeWidget::buildMenus()
{
  menus->buildMenus();
}

void
MainTreeWidget::treeItemCollapsed(QTreeWidgetItem * item)
{
  item->setIcon(0, QIcon::fromTheme("folder-video"));
}

void
MainTreeWidget::treeItemExpanded(QTreeWidgetItem * item)
{
  item->setIcon(0, QIcon::fromTheme("folder-open"));
}

void
MainTreeWidget::seriesAction()
{
  QAction *action = dynamic_cast<QAction *>(sender());
  SeriesAction *saction = dynamic_cast<SeriesAction *>(action);

  if (!saction && !action)
    return ;

  foreach (QModelIndex index, selectedIndexes())
    if (saction)
      menus->seriesAction(cache, index, saction);
    else
      menus->miscAction(cache, index, action);
}

void
MainTreeWidget::contextMenuEvent(QContextMenuEvent * event)
{
  QModelIndex index = indexAt(event->pos());

  menus->exec(index, event->globalPos());
}

void
MainTreeWidget::setCurrentItem(int showId)
{
  QTreeWidget::setCurrentItem(showsItems[showId]);
}

void
MainTreeWidget::setCurrentItem(int showId, int season)
{
  QTreeWidget::setCurrentItem(seasonsItems[showId][season]);
}

void
MainTreeWidget::updateTree(ShowModel *shows, SeasonModel *seasons)
{
  for (int i = 0; i < shows->rowCount(); ++i) {
    qint64 showId = shows->data(i, ShowModel::Id).toInt();
    QTreeWidgetItem *show = showsItems[showId];

    if (!show)
      continue ;

    show->setData(0, ShowModel::Id, showId);
    show->setData(0, Qt::DisplayRole, shows->data(i, Qt::DisplayRole));
    show->setData(0, ShowModel::Type, "show");
    //show->setData(0, Qt::DecorationRole, shows->data(i, Qt::DecorationRole));
    show->setIcon(0, QIcon::fromTheme("folder-video"));

    seasons->setShowId(showId);

    for (int j = 0; j < seasons->rowCount(); ++j) {
      qint64 seasonId = seasons->data(j, SeasonModel::Id).toInt();
      QTreeWidgetItem *season = seasonsItems[showId][seasonId];

      season->setData(0, SeasonModel::Id, seasonId);
      season->setData(0, SeasonModel::ShowId, showId);
      season->setData(0, SeasonModel::Type, "season");
      season->setData(0, Qt::DisplayRole, seasons->data(j, Qt::DisplayRole));
      //season->setData(0, Qt::DecorationRole, seasons->data(j, Qt::DecorationRole));
      season->setIcon(0, QIcon::fromTheme("video-x-generic"));
    }
  }
}

void
MainTreeWidget::buildTree(ShowModel *shows, SeasonModel *seasons)
{
  QTreeWidgetItem *home;

  buildMenus();

  showsItems.clear();
  seasonsItems.clear();
  clear();

  home = new QTreeWidgetItem(this);
  home->setText(0, tr("Index"));
  home->setIcon(0, style()->standardIcon(QStyle::SP_DirHomeIcon));
  home->setData(0, ShowModel::Id, 0);

  showsItems[0] = home;

  for (int i = 0; i < shows->rowCount(); ++i) {
    QTreeWidgetItem *show;
    qint64 showId = shows->data(i, ShowModel::Id).toInt();

    show = new QTreeWidgetItem(this);

    showsItems[showId] = show;
    seasons->setShowId(showId);

    for (int j = 0; j < seasons->rowCount(); ++j) {
      QTreeWidgetItem *season;
      qint64 seasonId = seasons->data(j, SeasonModel::Id).toInt();

      season = new QTreeWidgetItem(show);

      seasonsItems[showId][seasonId] = season;
    }
  }
  updateTree(shows, seasons);
}

