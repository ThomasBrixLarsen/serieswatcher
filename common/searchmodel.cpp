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

#include "tvdb.h"
#include "searchmodel.h"
#include "downloadworker.h"

SearchModel::SearchModel(QObject *parent)
  : QAbstractListModel(parent)
{
  DownloadWorker *worker = DownloadWorker::sharedInstance();

  connect(worker, SIGNAL(dataReceived(Job *, const QByteArray &)),
	  this, SLOT(downloadFinished(Job *, const QByteArray &)));
  connect(worker, SIGNAL(downloadProgress(Job *, qint64, qint64)),
	  this, SLOT(downloadProgress(Job *, qint64, qint64)));
  connect(worker, SIGNAL(downloadFailed(Job *, const QString &)),
	  this, SLOT(downloadFailed(Job *, const QString &)));
}

SearchModel::~SearchModel()
{
  clear();
}

void
SearchModel::clear()
{
  qDeleteAll(shows);
  shows.clear();
  bannersJobs.clear();
  banners.clear();
}

void
SearchModel::setQuery(const QString & query)
{
  if (query == currentQuery)
    return ;

  beginRemoveRows(QModelIndex(), 0, shows.size() - 1);
  clear();
  endRemoveRows();

  currentQuery = query;
  if (query.isEmpty())
    return ;

  DownloadWorker *worker = DownloadWorker::sharedInstance();

  searchJob = worker->fetchSearchResults(query);

  emit searchStarted();
}

int
SearchModel::rowCount(const QModelIndex & parent) const
{
  Q_ASSERT(!parent.isValid());
  return shows.size();
}

void
SearchModel::fetchBanner(QtTvDB::Show *show) const
{
  QtTvDB::Mirrors *mirrors = TvDB::mirrors();
  DownloadWorker *worker = DownloadWorker::sharedInstance();
  Job *job;

  if (banners.contains(show->id()) || bannersJobs.key(show))
    return ;

  if (show->banner().isEmpty())
    return ;

  job = worker->fetchBanner(show->id(), mirrors->bannerUrl(show->banner()));

  bannersJobs[job] = show;
}

QVariant
SearchModel::data(const QModelIndex & index, int role) const
{
  QtTvDB::Show *show;

  if (!index.isValid())
    return QVariant();

  Q_ASSERT(index.column() == 0);

  show = shows.at(index.row());

  if (!show)
    return QVariant();

  if (role == Qt::DisplayRole)
    return show->name();
  else if (role == Qt::DecorationRole) {
    if (banners.contains(show->id()))
      return banners[show->id()];

    fetchBanner(show);
    return QVariant();
  } else if (role == Qt::ToolTipRole) {
    QString descr = show->overview();

    if (descr.size() >= 64) {
      descr.truncate(64);
      descr.append("...");
    }

    return descr;
  } else if (role == ShowRole)
    return QVariant::fromValue((void *)show);

  return QVariant();
}

void
SearchModel::downloadFinished(Job *job, const QByteArray & data)
{
  if (job == searchJob) {
    searchResultsReceived(data);
    searchJob = NULL;
  }
  if (bannersJobs.contains(job)) {
    QtTvDB::Show *show = bannersJobs.take(job);
    bannerReceived(show, data);
  }
}

void
SearchModel::downloadProgress(Job *job, qint64 done, qint64 total)
{
  if (job == searchJob)
    emit searchProgress(done, total);
}

void
SearchModel::downloadFailed(Job *job, const QString & error)
{
  if (job == searchJob)
    emit searchFailed(error);
}

void
SearchModel::searchResultsReceived(const QByteArray & data)
{
  QList < QtTvDB::Show * > result;

  result = QtTvDB::Show::parseShows(data);

  if (result.size()) {
    beginInsertRows(QModelIndex(), 0, result.size() - 1);
    shows = result;
    endInsertRows();
  }

  emit searchDone(!!result.size());
}

void
SearchModel::bannerReceived(QtTvDB::Show *show, const QByteArray & data)
{
  int id;
  QPixmap pixmap;

  id = show->id();

  if (banners.contains(id))
    return ;

  pixmap.loadFromData(data);
  banners[id] = pixmap;
  //pixmap.scaled(QSize(256, 64), Qt::KeepAspectRatio, Qt::SmoothTransformation);

  QModelIndex idx = index(shows.indexOf(show));
  emit dataChanged(idx, idx);
}
