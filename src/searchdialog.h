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

#ifndef SEARCHDIALOG_H
# define SEARCHDIALOG_H

#include <QtCore/QMap>
#include <QtNetwork/QNetworkReply>

#include "tvdb.h"
#include "ui_searchdialog.h"

class QNetworkAccessManager;

class SearchDialog : public QDialog, private Ui_searchDialog
{
  Q_OBJECT
public:
  SearchDialog(QWidget * parent = 0);
  ~SearchDialog();
signals:
  void showSelected(const QString & name, qint64 id);

public slots:
  void accept();

protected:
  virtual void showEvent(QShowEvent * event);
  virtual void closeEvent(QCloseEvent * event);

private slots:
  void search();
  void requestFinished(QNetworkReply *rep);
  void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
  void error(QNetworkReply::NetworkError code);

private:
  void clear();
  void setItemIcon(QListWidgetItem *item, QtTvDB::Show *show);
  QNetworkAccessManager *manager;
  QMap < QNetworkReply *, QListWidgetItem * > iconReplies;
  QMap < QListWidgetItem *, QtTvDB::Show * > itemsShows;
  QPixmap blank;
};

#endif
