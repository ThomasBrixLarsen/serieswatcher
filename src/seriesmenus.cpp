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

#include "seriesmenus.h"
#include "seriesaction.h"
#include "tvdbcache.h"
#include "episodemodel.h"
#include "showmodel.h"
#include "seasonmodel.h"

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
  markNotWatchedAction = new QAction(QIcon(), QObject::tr("Mark as not watched"), parent);
  detailsAction = new QAction(QIcon::fromTheme("view-list-details"), QObject::tr("Details..."), parent);

  connect(updateShowAction, SIGNAL(triggered()), parent, SLOT(seriesAction()));
  connect(deleteShowAction, SIGNAL(triggered()), parent, SLOT(seriesAction()));
  connect(markWatchedAction, SIGNAL(triggered()), parent, SLOT(seriesAction()));
  connect(markNotWatchedAction, SIGNAL(triggered()), parent, SLOT(seriesAction()));
  connect(detailsAction, SIGNAL(triggered()), parent, SLOT(seriesAction()));

  episodeMenu = new QMenu(parent);
  //episodeMenu->addAction(updateShowAction);
  //episodeMenu->addAction(deleteShowAction);
  episodeMenu->addAction(markWatchedAction)
;  episodeMenu->addAction(markNotWatchedAction);
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
SeriesMenus::miscAction(TvDBCache *cache, const QModelIndex & index, QAction *action)
{
  QString type = index.data(Qt::UserRole).toString();

  int showId = -1;
  int id = -1;
  int season = -1;

  if (!index.isValid())
    return ;

  if (type == "episode") {
    id = index.data(EpisodeModel::Id).toInt();
    showId = index.data(EpisodeModel::ShowId).toInt();
    season = index.data(EpisodeModel::Season).toInt();
  } else if (type == "season") {
    showId = index.data(SeasonModel::ShowId).toInt();
    season = index.data(SeasonModel::Id).toInt();
    id = season;
  } else if (type == "show") {
    showId = index.data(ShowModel::Id).toInt();
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

    if (type == "episode")
      emit episodeWatched(id, watched);
    else
      emit episodesWatched(showId, season, watched);
  }
  if (action == detailsAction) {
    if (type == "episode")
      emit episodeDetails(id);
    if (type == "show")
      emit showDetails(id);
  }
}

void
SeriesMenus::seriesAction(TvDBCache *cache, const QModelIndex & index, SeriesAction *action)
{
  QString type = index.data(Qt::UserRole).toString();

  if (type == "episode") {
    QtTvDB::Episode *episode = cache->fetchEpisode(index.data(EpisodeModel::Id).toInt());
    QtTvDB::Show *show = cache->fetchShow(episode->showId());

    action->actEpisode(show, episode->season(), episode);
  }
  if (type == "season") {
    QtTvDB::Show *show = cache->fetchShow(index.data(SeasonModel::ShowId).toInt());
    int season = index.data(SeasonModel::Id).toInt();

    action->actSeason(show, season);
  }
  if (type == "show") {
    QtTvDB::Show *show = cache->fetchShow(index.data(ShowModel::Id).toInt());

    action->actShow(show);
  }
}

QAction *
SeriesMenus::exec(const QModelIndex & index, const QPoint & pos)
{
  QString type = index.data(Qt::UserRole).toString();

  if (type == "episode")
    return episodeMenu->exec(pos);
  if (type == "season")
    return seasonMenu->exec(pos);
  if (type == "show")
    return showMenu->exec(pos);
  return NULL;
}
