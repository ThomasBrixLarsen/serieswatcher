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

#include "config.h"

#include <QtCore/QTimer>
#include <QtCore/QTemporaryFile>
#include <QtSql/QSqlDatabase>

#include <QtTvDB>

#ifdef HAVE_LIBZIP
#include <zip.h>
#elif defined(HAVE_QUAZIP)
#include <quazip/quazip.h>
#include <quazip/quazipfile.h>
#endif

#include "downloadworker.h"
#include "updateworker.h"
#include "tvdb.h"
#include "tvdbcache.h"
#include "settings.h"

UpdateWorker::UpdateWorker(QObject *parent)
  : QObject(parent)
{
  mirrors = TvDB::mirrors();
  cache = new TvDBCache("updateworker");
}

UpdateWorker::~UpdateWorker()
{
  delete cache;

  QSqlDatabase::database("updateworker").close();
  QSqlDatabase::removeDatabase("updateworker");
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
  else if (job->type == Job::BannersXml)
    parseBannersXml(job);
  else if (job->type == Job::ShowAndEpisodesXml)
    parseShowAndEpisodesXml(job);

  if (job->type == Job::ShowAndEpisodesZip ||
      job->type == Job::BannersXml ||
      job->type == Job::ShowAndEpisodesXml)
    emit databaseUpdated();

  emit parseProgress(job, 1, 1);

  job->state = Job::Finished;

  emit parseFinished(job);
}

#ifdef HAVE_LIBZIP
void
UpdateWorker::parseShowAndEpisodesZip(Job *job)
{
  int err = 0;
  QTemporaryFile file;

  file.open();
  file.write(job->data);
  file.flush();

  struct zip *zip = zip_open(file.fileName().toLatin1().constData(), 0, &err);

  if (!zip) {
    emit error(tr("Error"), tr("Zip open error: %1").arg(err));
    return ;
  }

  int files = zip_get_num_files(zip);

  for (int index = 0; index < files; ++index) {
    const char *filename = zip_get_name(zip, index, 0);
    struct zip_stat st;
    struct zip_file *fp;

    if (!filename)
      continue;
    if (QLatin1String(filename) == QLatin1String("actors.xml"))
      continue;

    zip_stat_index(zip, index, 0, &st);
    fp = zip_fopen_index(zip, index, 0);

    if (!fp) {
      emit error(tr("Error"), tr("Zip error: %1").arg(zip_file_strerror(fp)));
      zip_close(zip);
      return ;
    }

    job->data = QByteArray();
    ssize_t bytes = 0;
    while (bytes < st.size) {
      char buf[2048];
      ssize_t ret = zip_fread(fp, buf, sizeof (buf));

      if (ret == -1) {
	emit error(tr("Error"), tr("Zip error: %1").arg(zip_file_strerror(fp)));
	zip_close(zip);
	return ;
      }
      job->data.append(buf, ret);
      bytes += ret;
    }

    if (QLatin1String(filename) == QLatin1String("banners.xml"))
      parseBannersXml(job);
    else
      parseShowAndEpisodesXml(job);

    zip_fclose(fp);
  }
  zip_close(zip);
}
#elif defined(HAVE_QUAZIP)
void
UpdateWorker::parseShowAndEpisodesZip(Job *job)
{
  QTemporaryFile file;

  file.open();
  file.write(job->data);
  file.flush();

  QuaZip zip(file.fileName());

  if (!zip.open(QuaZip::mdUnzip)) {
    emit error(tr("Error"), tr("Zip error: %1").arg(zip.getZipError()));
    return ;
  }
  if (!zip.isOpen()) {
    emit error(tr("Error"), tr("Zip open error: %1").arg(zip.getZipError()));
    return ;
  }
  for(bool more = zip.goToFirstFile(); more; more = zip.goToNextFile()) {
    if (zip.getCurrentFileName() == "actors.xml")
      continue;

    QuaZipFile file(&zip);
    file.open(QIODevice::ReadOnly);
    job->data = file.readAll();
    if (zip.getCurrentFileName() == "banners.xml")
      parseBannersXml(job);
    else
      parseShowAndEpisodesXml(job);
    file.close();
  }
  if(zip.getZipError() != UNZ_OK) {
    emit error(tr("Error"), tr("Zip error: %1").arg(zip.getZipError()));
  }
}
#else
void
UpdateWorker::parseShowAndEpisodesZip(Job *job)
{
}
#endif

void
UpdateWorker::parseBannersXml(Job *job)
{
  Settings settings;
  QList < QtTvDB::Banner * > banners = QtTvDB::Banner::parseBanners(job->data);
  bool prefetch = settings.value("prefetchBanners").toBool();

  cache->storeBanners(banners, job->id);

  if (prefetch) {
    foreach ( QtTvDB::Banner * banner, banners) {
      if (banner->language() != "en")
	continue ;
      if (banner->type() == "poster" || banner->type() == "series" ||
	  (banner->type() == "season" && banner->type2() == "season"))
	bannerJob(banner->id(), TvDBCache::BannerTypePoster, mirrors->bannerUrl(banner->path()));
    }
  }

  qDeleteAll(banners);
  cache->sync();
}

void
UpdateWorker::parseShowAndEpisodesXml(Job *job)
{
  Settings settings;
  QList < QtTvDB::Show * > shows = QtTvDB::Show::parseShows(job->data);
  QList < QtTvDB::Episode * > episodes = QtTvDB::Episode::parseEpisodes(job->data);
  bool prefetch = settings.value("prefetchBanners").toBool();

  qint64 total = shows.size() + episodes.size();
  qint64 done = 0;

  foreach (QtTvDB::Show *show, shows) {
    cache->storeShow(show);
    emit parseProgress(job, done++, total);
  }


  foreach (QtTvDB::Episode *episode, episodes) {
    if (prefetch && episode->language() == "en" && !episode->image().isEmpty())
      bannerJob(episode->id(), TvDBCache::BannerTypeEpisode, mirrors->bannerUrl(episode->image()));

    cache->storeEpisode(episode);
    emit parseProgress(job, done++, total);
  }

  qDeleteAll(shows);
  qDeleteAll(episodes);

  cache->sync();
}

void
UpdateWorker::bannerJob(qint64 id, TvDBCache::BannerType type, const QUrl & url)
{
  Job *bjob;

  bjob = new Job();

  bjob->id = id;
  bjob->type = Job::Banner;
  bjob->bannerType = type;
  bjob->url = url;

  emit newJob(bjob);
}

void
UpdateWorker::abort()
{
  QMutexLocker locker(&working);

  jobs.clear();
}
