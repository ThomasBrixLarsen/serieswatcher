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

#ifndef TVDB_CACHE_H
# define TVDB_CACHE_H

#include <QtTvDB>

class TvDBCache
{
public:
  TvDBCache();
  ~TvDBCache();

  void storeShow(QtTvDB::Show *show);
  void storeEpisode(QtTvDB::Episode *episode);
  void storeEpisodes(QList < QtTvDB::Episode * > episodes);
  void storeBanner(QtTvDB::Banner *banner);
  void storeBanners(QList < QtTvDB::Banner * > banners);
  void storeBannerFile(qint64 id, const QByteArray &data);

  QtTvDB::Show *fetchShow();
  QtTvDB::Episode *fetchEpisode();
  QList < QtTvDB::Episode * > fetchEpisodes(qint64 showId, qint64 seasonId = -1);
  QtTvDB::Banner *fetchBanner(qint64 id);
  QList < QtTvDB::Banner * > fetchBanners(qint64 showId);

  bool hasBannerFile(qint64 id);
  QPixmap fetchBannerFile(qint64 id);

 private:
  QString bannerPath(qint64 id);
};

#endif
