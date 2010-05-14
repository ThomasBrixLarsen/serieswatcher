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

#include <QtCore/QList>

#include "seriesaction.h"
#include "settings.h"

SeriesAction::SeriesAction(QObject * parent)
  : QAction(parent)
{
}

SeriesAction::SeriesAction(QAction * action)
  : QAction(action->icon(), action->text(), action->parent())
{
}

SeriesAction::~SeriesAction()
{
}


QUrl
SeriesAction::buildShowUrl(QtTvDB::Show *show) const
{
  return replaceModifiers(showurl, show);
}

QUrl
SeriesAction::buildSeasonUrl(QtTvDB::Show *show, int season) const
{
  return replaceModifiers(seasonurl, show, season);
}

QUrl
SeriesAction::buildEpisodeUrl(QtTvDB::Show *show, int season, QtTvDB::Episode *episode) const
{
  return replaceModifiers(episodeurl, show, season, episode);
}

QList < SeriesAction * >
SeriesAction::fromSettings(void)
{
  QList < SeriesAction * > actions;
  Settings settings;

  int size = settings.beginReadArray("Actions");
  for (int i = 0; i < size; ++i) {
    SeriesAction *action = new SeriesAction();

    settings.setArrayIndex(i);

    action->setText(settings.value("text").toString());
    action->setIcon(QPixmap(settings.value("icon").toString()));
    action->setShowUrl(settings.value("showUrl").toString());
    action->setSeasonUrl(settings.value("seasonUrl").toString());
    action->setEpisodeUrl(settings.value("episodeUrl").toString());
    actions.append(action);
  }

  settings.endArray();
  return actions;
}

void
SeriesAction::addToSettings(QList < SeriesAction * > actions)
{
  Settings settings;

  settings.beginWriteArray("Actions");
  for (int i = 0; i < actions.size(); ++i) {
    settings.setArrayIndex(i);
    settings.setValue("text", actions.at(i)->text());
    settings.setValue("icon", actions.at(i)->iconPath());
    settings.setValue("showUrl", actions.at(i)->showUrl().toString());
    settings.setValue("seasonUrl", actions.at(i)->seasonUrl().toString());
    settings.setValue("episodeUrl", actions.at(i)->episodeUrl().toString());
  }
  settings.endArray();
}

void
SeriesAction::addToSettings(SeriesAction *action)
{
}

void
SeriesAction::removeFromSettings(SeriesAction *action)
{
}


QUrl
SeriesAction::replaceModifiers(QUrl url, QtTvDB::Show *show,
			       int season, QtTvDB::Episode *episode) const
{
  return url;
}
