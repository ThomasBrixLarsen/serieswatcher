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

#ifndef SEARCHMODEL_H
# define SEARCHMODEL_H

#include <QtCore/QMap>
#include <QtCore/QAbstractListModel>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>
#include <QtNetwork/QNetworkReply>

#include "tvdb.h"

class QNetworkReply;

class SearchModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum {
      ShowRole = Qt::UserRole,
    } Roles;

    SearchModel(QObject *parent = 0);
    ~SearchModel();

    void setQuery(const QString & query);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role) const;

signals:
    void searchStarted();
    void searchProgress(qint64 bytesReceived, qint64 bytesTotal);
    void searchDone(bool ok);
    void networkError(QNetworkReply::NetworkError code);

private slots:
    void searchResultsReceived();
    void bannerReceived();

private:
    void clear();
    void fetchBanner(QtTvDB::Show *show) const;

    QList < QtTvDB::Show * > shows;
    mutable QMap < QNetworkReply * , QtTvDB::Show * > bannersReplies;
    QMap < int, QIcon > banners;
    QString currentQuery;
};

#endif
