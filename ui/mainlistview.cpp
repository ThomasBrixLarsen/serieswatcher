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

#include <QtGui/QContextMenuEvent>

#include "mainlistview.h"
#include "seriesaction.h"
#include "tvdbcache.h"

MainListView::MainListView(QWidget *parent)
  : QListView(parent)
{
  menus = new SeriesMenus(this);
  buildMenus();
}

MainListView::~MainListView()
{
  delete menus;
}

void
MainListView::buildMenus()
{
  menus->buildMenus();
}

void
MainListView::seriesAction()
{
  QAction *action = dynamic_cast<QAction *>(sender());
  SeriesAction *saction = dynamic_cast<SeriesAction *>(action);

  if (!saction && !action)
    return ;

  foreach (QModelIndex index, selectedIndexes())
    if (saction)
      menus->seriesAction(index, saction);
    else
      menus->miscAction(index, action);
}

void
MainListView::contextMenuEvent(QContextMenuEvent * event)
{
  QModelIndex index = indexAt(event->pos());

  menus->exec(index, event->globalPos());
}
