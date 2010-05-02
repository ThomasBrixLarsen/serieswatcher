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

#include <QtCore/QTimer>
#include <QtSql/QSqlDatabase>

#include <QtTvDB>

#include "downloadworker.h"
#include "updateworker.h"
#include "tvdb.h"
#include "tvdbcache.h"

UpdateWorker::UpdateWorker(QObject *parent)
  : QObject(parent)
{
  mirrors = TvDB::mirrors();
  cache = new TvDBCache("updateworker");
}

UpdateWorker::~UpdateWorker()
{
  QSqlDatabase::database("updateworker").close();
  QSqlDatabase::removeDatabase("updateworker");

  delete cache;
}

/* This method is should be called from another thread */
void
UpdateWorker::startJob(Job *job)
{
  QMutexLocker locker(&working);

  job->state = Job::Parsing;

  emit parseStarted(job);

  emit parseProgress(job, 0, 1);

  if (job->type == Job::ShowAndEpisodesZip)
    parseShowAndEpisodesZip(job);
  else if (job->type == Job::Banner)
    parseBanner(job);
  else if (job->type == Job::BannersXml)
    parseBannersXml(job);
  else if (job->type == Job::ShowAndEpisodesXml)
    parseShowAndEpisodesXml(job);

  emit parseProgress(job, 1, 1);

  job->state = Job::Finished;

  emit parseFinished(job);
}

void
UpdateWorker::parseShowAndEpisodesZip(Job *job)
{
}

void
UpdateWorker::parseBanner(Job *job)
{
  cache->storeBannerFile(job->id, TvDBCache::Poster, job->data);
}

void
UpdateWorker::parseBannersXml(Job *job)
{
  QList < QtTvDB::Banner * > banners = QtTvDB::Banner::parseBanners(job->data);

  cache->storeBanners(banners, job->id);

  foreach ( QtTvDB::Banner * banner, banners) {
    if (banner->language() != "en")
      continue ;
    if (banner->type() == "poster" || (banner->type() == "season" && banner->type2() == "season")) {
      if (cache->hasBannerFile(banner->id(), TvDBCache::Poster))
	continue ;

      Job *bjob = new Job();

      bjob->id = banner->id();
      bjob->type = Job::Banner;
      bjob->url = mirrors->bannerUrl(banner->path());

      emit newJob(bjob);
    }
  }
  qDeleteAll(banners);

  cache->sync();
}

void
UpdateWorker::parseShowAndEpisodesXml(Job *job)
{
  QList < QtTvDB::Show * > shows = QtTvDB::Show::parseShows(job->data);
  QList < QtTvDB::Episode * > episodes = QtTvDB::Episode::parseEpisodes(job->data);

  qint64 total = shows.size() + episodes.size();
  qint64 done = 0;

  foreach (QtTvDB::Show *show, shows) {
    cache->storeShow(show);
    emit parseProgress(job, done++, total);
  }

  foreach (QtTvDB::Episode *episode, episodes) {
    cache->storeEpisode(episode);
    emit parseProgress(job, done++, total);
  }

  qDeleteAll(shows);
  qDeleteAll(episodes);

  cache->sync();
}


void
UpdateWorker::abord()
{
  QMutexLocker locker(&working);

  jobs.clear();
}
