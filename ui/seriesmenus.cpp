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

#include "seriesmenus.h"
#include "seriesaction.h"
#include "tvdbcache.h"
#include "tvdbmodel.h"

SeriesMenus::SeriesMenus(QWidget *p)
  : QObject(p), parent(p)
{
  episodeMenu = seasonMenu = showMenu = NULL;
  updateShowAction = deleteShowAction = markWatchedAction = NULL;
  markNotWatchedAction = detailsAction = NULL;
}

SeriesMenus::~SeriesMenus()
{
}


void
SeriesMenus::buildMenus()
{
  delete episodeMenu;
  delete seasonMenu;
  delete showMenu;
  delete updateShowAction;
  delete deleteShowAction;
  delete markWatchedAction;
  delete markNotWatchedAction;
  delete detailsAction;

  updateShowAction = new QAction(QIcon::fromTheme("download"), QObject::tr("Update Show"), parent);
  deleteShowAction = new QAction(QIcon::fromTheme("edit-delete"), QObject::tr("Delete Show"), parent);
  markWatchedAction = new QAction(QIcon::fromTheme("checkbox"), QObject::tr("Mark as watched"), parent);
  markNotWatchedAction = new QAction(QIcon::fromTheme("dialog-cancel"), QObject::tr("Mark as not watched"), parent);
  detailsAction = new QAction(QIcon::fromTheme("view-list-details"), QObject::tr("Details..."), parent);

  connect(updateShowAction, SIGNAL(triggered()), parent, SLOT(seriesAction()));
  connect(deleteShowAction, SIGNAL(triggered()), parent, SLOT(seriesAction()));
  connect(markWatchedAction, SIGNAL(triggered()), parent, SLOT(seriesAction()));
  connect(markNotWatchedAction, SIGNAL(triggered()), parent, SLOT(seriesAction()));
  connect(detailsAction, SIGNAL(triggered()), parent, SLOT(seriesAction()));

  episodeMenu = new QMenu(parent);
  //episodeMenu->addAction(updateShowAction);
  //episodeMenu->addAction(deleteShowAction);
  episodeMenu->addAction(markWatchedAction);
  episodeMenu->addAction(markNotWatchedAction);
  episodeMenu->addAction(detailsAction);
  episodeMenu->addSeparator();

  seasonMenu = new QMenu(parent);
  //seasonMenu->addAction(updateShowAction);
  //seasonMenu->addAction(deleteShowAction);
  seasonMenu->addAction(markWatchedAction);
  seasonMenu->addAction(markNotWatchedAction);
  //seasonMenu->addAction(detailsAction);
  seasonMenu->addSeparator();

  showMenu = new QMenu(parent);
  showMenu->addAction(updateShowAction);
  showMenu->addAction(deleteShowAction);
  showMenu->addSeparator();
  showMenu->addAction(markWatchedAction);
  showMenu->addAction(markNotWatchedAction);
  showMenu->addAction(detailsAction);
  showMenu->addSeparator();

  foreach (SeriesAction *action, SeriesAction::fromSettings()) {
    connect(action, SIGNAL(triggered()), parent, SLOT(seriesAction()));

    if (!action->showUrl().isEmpty())
      showMenu->addAction(action);
    if (!action->seasonUrl().isEmpty())
      seasonMenu->addAction(action);
    if (!action->episodeUrl().isEmpty())
      episodeMenu->addAction(action);
  }
}

void
SeriesMenus::miscAction(const QModelIndex & index, QAction *action)
{
  int type = index.data(TvDBModel::Type).toInt();

  int showId = -1;
  int id = -1;
  int season = -1;

  if (!index.isValid())
    return ;

  if (type == TvDBModel::Episode) {
    id = index.data(TvDBModel::Id).toInt();
    showId = index.parent().parent().data(TvDBModel::Id).toInt();
    season = index.parent().data(TvDBModel::Name).toInt();
  } else if (type == TvDBModel::Season) {
    showId = index.parent().data(TvDBModel::Id).toInt();
    season = index.data(TvDBModel::Name).toInt();
    id = season;
  } else if (type == TvDBModel::Show) {
    showId = index.data(TvDBModel::Id).toInt();
    id = showId;
  } else {
    return ;
  }

  if (action == updateShowAction)
    emit updateShow(showId);
  if (action == deleteShowAction)
    emit deleteShow(showId);
  if (action == markWatchedAction || action == markNotWatchedAction) {
    bool watched = action == markWatchedAction;

    emit episodesWatched(index, watched);
  }

  if (action == detailsAction) {
    if (type == TvDBModel::Episode)
      emit episodeDetails(id);
    if (type == TvDBModel::Show)
      emit showDetails(id);
  }
}

void
SeriesMenus::seriesAction(const QModelIndex & index, SeriesAction *action)
{
  TvDBCache *cache = TvDBCache::instance();
  int type = index.data(TvDBModel::Type).toInt();

  if (type == TvDBModel::Episode) {
    QtTvDB::Episode *episode = cache->fetchEpisode(index.data(TvDBModel::Id).toInt());
    QtTvDB::Show *show = cache->fetchShow(episode->showId());

    action->actEpisode(show, episode->season(), episode);
  }
  if (type == TvDBModel::Season) {
    QtTvDB::Show *show = cache->fetchShow(index.parent().data(TvDBModel::Id).toInt());
    int season = index.data(TvDBModel::Name).toInt();

    action->actSeason(show, season);
  }
  if (type == TvDBModel::Show) {
    QtTvDB::Show *show = cache->fetchShow(index.data(TvDBModel::Id).toInt());

    action->actShow(show);
  }
}

QAction *
SeriesMenus::exec(const QModelIndex & index, const QPoint & pos)
{
  int type = index.data(TvDBModel::Type).toInt();

  if (type == TvDBModel::Episode)
    return episodeMenu->exec(pos);
  if (type == TvDBModel::Season)
    return seasonMenu->exec(pos);
  if (type == TvDBModel::Show)
    return showMenu->exec(pos);
  return NULL;
}
