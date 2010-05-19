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

#ifndef SERIESACTION_H
# define SERIESACTION_H

#include <QtGui/QAction>
#include <QtCore/QUrl>
#include <QtTvDB>

class SeriesAction : public QAction {
 Q_OBJECT
public:
  SeriesAction(QObject * parent = 0);
  SeriesAction(QAction * action);
  ~SeriesAction();

  void actUrl(const QUrl & url) const;
  void actShow(QtTvDB::Show *show) const;
  void actSeason(QtTvDB::Show *show, int season) const;
  void actEpisode(QtTvDB::Show *show, int season, QtTvDB::Episode *episode) const;

  QUrl buildShowUrl(QtTvDB::Show *show) const;
  QUrl buildSeasonUrl(QtTvDB::Show *show, int season) const;
  QUrl buildEpisodeUrl(QtTvDB::Show *show, int season, QtTvDB::Episode *episode) const;

  QUrl showUrl() const { return showurl; }
  QUrl seasonUrl() const { return seasonurl; }
  QUrl episodeUrl() const { return episodeurl; }
  QString iconPath() const { return iconpath; }

  void setShowUrl(const QUrl & url) { showurl = url; }
  void setSeasonUrl(const QUrl & url) { seasonurl = url; }
  void setEpisodeUrl(const QUrl & url) { episodeurl = url; }
  void setIconPath(const QString & path) { iconpath = path; setIcon(QPixmap(path)); } 

  static QList < SeriesAction * > fromSettings(void);
  static void addToSettings(QList < SeriesAction * > actions);

private:
  QUrl replaceModifiers(QUrl url, QtTvDB::Show *show = NULL,
			int season = -1, QtTvDB::Episode *episode = NULL) const;


  QUrl showurl;
  QUrl seasonurl;
  QUrl episodeurl;
  QString iconpath;
};

#endif
