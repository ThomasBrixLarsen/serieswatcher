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

#ifndef TVDB_CACHE_H
# define TVDB_CACHE_H

#include <QtSql/QSqlDatabase>
#include <QtCore/QSize>

#include <QtTvDB>

class TvDBCache : public QObject
{
  Q_OBJECT

public:
  struct Show
  {
    int seasons;
    int episodes;
    int episodesWatched;
    int episodesNew;
    QtTvDB::Episode *nextEpisode;
  };

  struct Season
  {
    int season;
    QString name;
    int episodes;
    int episodesWatched;
    int episodesNew;
    QUrl banner;
    QUrl bannerWide;
    QtTvDB::Episode *nextEpisode;
  };

  struct Episode
  {
    bool watched;
    bool hot;
  };

  enum BannerType {
    BannerTypePoster,
    BannerTypeBanner,
    BannerTypeSeason,
    BannerTypeSeasonWide,
    BannerTypeSearch,
    BannerTypeEpisode
  };

  TvDBCache(const QString & name = QLatin1String(QSqlDatabase::defaultConnection));
  ~TvDBCache();

  static TvDBCache *instance();

  void episodesWatched(qint64 showId, int season, bool watched);
  void episodeWatched(qint64 id, bool watched);
  void deleteShow(qint64 showId);

  void storeShow(QtTvDB::Show *show);
  void storeShows(QList < QtTvDB::Show * > shows);
  void storeEpisode(QtTvDB::Episode *episode);
  void storeEpisodes(QList < QtTvDB::Episode * > episodes);
  void storeBanner(QtTvDB::Banner *banner, qint64 showId);
  void storeBanners(QList < QtTvDB::Banner * > banners, qint64 showId);

  QtTvDB::Show *fetchShow(qint64 id);
  QList < QtTvDB::Show * > fetchShows();

  QtTvDB::Episode *fetchEpisode(qint64 id);
  QList < QtTvDB::Episode * > fetchEpisodes(qint64 showId, qint64 seasonId = -1);

  QtTvDB::Banner *fetchBanner(qint64 id);
  QList < QtTvDB::Banner * > fetchBanners(qint64 showId);

  QList < Show * > fetchShowsExtras();
  QList < Season * > fetchSeasonsExtras(qint64 showId);
  QList < Episode * > fetchEpisodesExtras(qint64 showId, qint64 season);

  QString name();
  void sync();

 signals:
  void cacheChanged(qint64 showId, qint64 seasonId, qint64 episodeId);

 private:
  bool connectDb(const QString & name = QLatin1String(QSqlDatabase::defaultConnection));
  QtTvDB::Show *showFromRecord(QSqlRecord record);
  QtTvDB::Episode *episodeFromRecord(QSqlRecord record);
  QtTvDB::Banner *bannerFromRecord(QSqlRecord record);

 private:
  static TvDBCache *_instance;

  QSqlDatabase db;
  QString dbName;
};

#endif
