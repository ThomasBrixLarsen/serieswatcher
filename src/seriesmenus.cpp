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

SeriesMenus::SeriesMenus()
{
  episodeMenu = seasonMenu = showMenu = NULL;
}

SeriesMenus::~SeriesMenus()
{
}


void
SeriesMenus::buildMenus(QWidget *parent)
{
  delete episodeMenu;
  delete seasonMenu;
  delete showMenu;

  QAction *updateShowAction = new QAction(QIcon::fromTheme("download"), QObject::tr("Update Show"), parent);
  QAction *deleteShowAction = new QAction(QIcon::fromTheme("edit-delete"), QObject::tr("Delete Show"), parent);
  QAction *markWatchedAction = new QAction(QIcon::fromTheme("checkbox"), QObject::tr("Mark Watched"), parent);

  episodeMenu = new QMenu(parent);
  //episodeMenu->addAction(updateShowAction);
  //episodeMenu->addAction(deleteShowAction);
  episodeMenu->addAction(markWatchedAction);
  episodeMenu->addSeparator();

  seasonMenu = new QMenu(parent);
  //seasonMenu->addAction(updateShowAction);
  //seasonMenu->addAction(deleteShowAction);
  seasonMenu->addAction(markWatchedAction);
  seasonMenu->addSeparator();

  showMenu = new QMenu(parent);
  showMenu->addAction(updateShowAction);
  showMenu->addAction(deleteShowAction);
  showMenu->addAction(markWatchedAction);
  showMenu->addSeparator();

  foreach (SeriesAction *action, SeriesAction::fromSettings()) {
    QObject::connect(action, SIGNAL(triggered()), parent, SLOT(seriesAction()));

    if (!action->showUrl().isEmpty())
      showMenu->addAction(action);
    if (!action->seasonUrl().isEmpty())
      seasonMenu->addAction(action);
    if (!action->episodeUrl().isEmpty())
      episodeMenu->addAction(action);
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
SeriesMenus::execMenu(const QModelIndex & index, const QPoint & pos)
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
