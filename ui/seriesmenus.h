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

#ifndef SERIESMENUS_H
# define SERIESMENUS_H

#include <QtGui/QMenu>
#include <QtCore/QModelIndex>

class SeriesAction;
class TvDBCache;

class SeriesMenus : public QObject {
 Q_OBJECT
public:
  SeriesMenus(QWidget *parent = 0);
  ~SeriesMenus();

  void buildMenus();
  void miscAction(const QModelIndex & index, QAction *action);
  void seriesAction(const QModelIndex & index, SeriesAction *action);
  QAction *exec(const QModelIndex & index, const QPoint & pos);

 signals:
  void updateShow(qint64 showId);
  void deleteShow(qint64 showId);
  void episodesWatched(const QModelIndex & index, bool watched);
  void episodeDetails(qint64 id);
  void showDetails(qint64 id);

 private:
  TvDBCache *cache;
  QWidget *parent;
  QMenu *episodeMenu;
  QMenu *seasonMenu;
  QMenu *showMenu;
  QAction *updateShowAction;
  QAction *deleteShowAction;
  QAction *markWatchedAction;
  QAction *markNotWatchedAction;
  QAction *detailsAction;
};

#endif
