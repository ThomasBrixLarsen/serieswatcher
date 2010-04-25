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

  QtTvDB::Show *fetchShow();
  QtTvDB::Episode *fetchEpisode();
  QList < QtTvDB::Episode * > fetchEpisodes(qint64 showId, qint64 seasonId = -1);
  QtTvDB::Banner *fetchBanner(qint64 id);
  QList < QtTvDB::Banner * > fetchBanners(qint64 showId);

};

#endif
