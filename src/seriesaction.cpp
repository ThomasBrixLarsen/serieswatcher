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
#include <QtGui/QDesktopServices>

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

void
SeriesAction::actUrl(const QUrl & url) const
{
  QDesktopServices::openUrl(url);
}

void
SeriesAction::actShow(QtTvDB::Show *show) const
{
  actUrl(buildShowUrl(show));
}

void
SeriesAction::actSeason(QtTvDB::Show *show, int season) const
{
  actUrl(buildSeasonUrl(show, season));
}

void
SeriesAction::actEpisode(QtTvDB::Show *show, int season, QtTvDB::Episode *episode) const
{
  actUrl(buildEpisodeUrl(show, season, episode));
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
    action->setIconPath(settings.value("icon").toString());
    action->setShowUrl(QUrl::fromUserInput(settings.value("showUrl").toString()));
    action->setSeasonUrl(QUrl::fromUserInput(settings.value("seasonUrl").toString()));
    action->setEpisodeUrl(QUrl::fromUserInput(settings.value("episodeUrl").toString()));
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

QUrl
SeriesAction::replaceModifiers(QUrl url, QtTvDB::Show *show,
			       int season, QtTvDB::Episode *episode) const
{
  /*
   * %S current show name        (MyShow)
   * %E current episode name     (Pilot)
   * %n current season number   (1)
   * %e current episode number  (1)
   * %b formated episode number (E01)
   * %m formated season number (S01)
   */
  QString str = url.toString();

  if (show) {
    str.replace("%S", QUrl::toPercentEncoding(show->name()));
  }
  if (season >= 0) {
    str.replace("%n", QUrl::toPercentEncoding(QString("%1").arg(season)));
    str.replace("%m", QUrl::toPercentEncoding(QString("S%1").arg(season, 2, 10, QLatin1Char('0'))));
  }
  if (episode) {
    str.replace("%E", QUrl::toPercentEncoding(episode->name()));
    str.replace("%e", QUrl::toPercentEncoding(QString("%1").arg(episode->episode())));
    str.replace("%b", QUrl::toPercentEncoding(QString("E%1").arg(episode->episode(), 2, 10, QLatin1Char('0'))));
  }
  QUrl ret;
  ret.setEncodedUrl(str.toUtf8());
  return ret;
}
