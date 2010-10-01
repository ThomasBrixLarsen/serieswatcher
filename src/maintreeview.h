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

#ifndef MAINTREEVIEW_H
# define MAINTREEVIEW_H

#include <QtGui/QTreeWidget>

#include "seriesmenus.h"

class TvDBCache;

class MainTreeView : public QTreeView {
  Q_OBJECT
public:
  MainTreeView(QWidget *parent = 0);
  virtual ~MainTreeView();

  void buildMenus();

  const SeriesMenus *getMenus() const { return menus; }

/* Needed by SeriesMenus */
private slots:
  void seriesAction();

protected:
  virtual void contextMenuEvent(QContextMenuEvent * event);

private:
  TvDBCache *cache;
  SeriesMenus *menus;
};

#endif
