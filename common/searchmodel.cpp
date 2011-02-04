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

#include <QtNetwork/QNetworkReply>

#include "tvdb.h"
#include "searchmodel.h"
#include "downloadworker.h"

SearchModel::SearchModel(QObject *parent)
  : QAbstractListModel(parent)
{
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
  bannersReplies.clear();
  banners.clear();
}

void
SearchModel::setQuery(const QString & query)
{
  if (query == currentQuery)
    return ;

  if (shows.size()) {
    beginRemoveRows(QModelIndex(), 0, shows.size() - 1);
    clear();
    endRemoveRows();
  }

  currentQuery = query;
  if (query.isEmpty())
    return ;

  DownloadWorker *worker = DownloadWorker::sharedInstance();
  QNetworkReply *reply = worker->fetchSearchResults(query);

  emit searchStarted();

  connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SIGNAL(searchProgress(qint64, qint64)));
  connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SIGNAL(networkError(QNetworkReply::NetworkError)));
  connect(reply, SIGNAL(finished()), this, SLOT(searchResultsReceived()));
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
  QNetworkReply *reply;

  if (banners.contains(show->id()) || bannersReplies.key(show))
    return ;

  if (show->banner().isEmpty())
    return ;

  reply = worker->fetchBanner(show->id(), mirrors->bannerUrl(show->banner()));
  connect(reply, SIGNAL(finished()), this, SLOT(bannerReceived()));

  bannersReplies[reply] = show;
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
SearchModel::searchResultsReceived(void)
{
  QNetworkReply *reply = dynamic_cast<QNetworkReply *>(sender());
  QList < QtTvDB::Show * > result;

  reply->deleteLater();

  result = QtTvDB::Show::parseShows(reply->readAll());

  if (result.size()) {
    beginInsertRows(QModelIndex(), 0, result.size() - 1);
    shows = result;
    endInsertRows();
  }

  emit searchDone(reply->error() == QNetworkReply::NoError);
}

void
SearchModel::bannerReceived(void)
{
  QNetworkReply *reply = dynamic_cast<QNetworkReply *>(sender());
  QtTvDB::Show *show;
  int id;
  QPixmap pixmap;

  reply->deleteLater();

  if (!bannersReplies.contains(reply))
    return ;

  show = bannersReplies[reply];
  id = show->id();

  if (banners.contains(id))
    return ;

  pixmap.loadFromData(reply->readAll());
  banners[id] = pixmap; //pixmap.scaled(QSize(256, 64), Qt::KeepAspectRatio, Qt::SmoothTransformation);

  QModelIndex idx = index(shows.indexOf(show));
  emit dataChanged(idx, idx);
}
