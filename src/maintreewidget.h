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

#ifndef MAINTREEWIDGET_H
# define MAINTREEWIDGET_H

#include <QtGui/QTreeWidget>

#include "seriesmenus.h"

class ShowModel;
class SeasonModel;
class TvDBCache;

class MainTreeWidget : public QTreeWidget {
  Q_OBJECT
public:
  MainTreeWidget(QWidget *parent = 0);
  virtual ~MainTreeWidget();

  void buildMenus();
  void buildTree(ShowModel *shows, SeasonModel *model);
  void updateTree(ShowModel *shows, SeasonModel *model);

  void setCurrentItem(int showId);
  void setCurrentItem(int showId, int seasonId);

  const SeriesMenus *getMenus() const { return menus; }

/* Needed by SeriesMenus */
private slots:
  void seriesAction();
  void treeItemCollapsed(QTreeWidgetItem * item);
  void treeItemExpanded(QTreeWidgetItem * item);

protected:
  virtual void contextMenuEvent(QContextMenuEvent * event);

private:
  SeriesMenus *menus;
  TvDBCache *cache;

  QMap < int, QTreeWidgetItem * > showsItems;
  QMap < int , QMap < int , QTreeWidgetItem * > > seasonsItems;
};

#endif

