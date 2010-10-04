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
#include <QtGui/QHeaderView>

#include "maintreeview.h"
#include "seriesaction.h"
#include "tvdbcache.h"
#include "tvdbmodel.h"

MainTreeView::MainTreeView(QWidget *parent)
  : QTreeView(parent)
{
  cache = new TvDBCache();
  menus = new SeriesMenus(this);

  buildMenus();
}

MainTreeView::~MainTreeView()
{
  delete cache;
  delete menus;
}

void
MainTreeView::buildMenus()
{
  menus->buildMenus();
}

void
MainTreeView::setModel(QAbstractItemModel *model)
{
  QTreeView::setModel(model);

  setColumnHidden(2, true);
  setColumnHidden(4, true);
  setColumnHidden(5, true);
  setColumnHidden(6, true);
  setColumnHidden(7, true);

  header()->setStretchLastSection(false);
  header()->setResizeMode(1, QHeaderView::ResizeToContents);
  header()->setResizeMode(3, QHeaderView::ResizeToContents);
  header()->setResizeMode(0, QHeaderView::Stretch);
}

void
MainTreeView::seriesAction()
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
MainTreeView::contextMenuEvent(QContextMenuEvent * event)
{
  QModelIndex index = indexAt(event->pos());

  menus->exec(index, event->globalPos());
}
