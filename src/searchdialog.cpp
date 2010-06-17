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

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtGui/QPixmapCache>
#include <QtGui/QMessageBox>

#include "searchdialog.h"
#include "tvdb.h"
#include "tvdbcache.h"

SearchDialog::SearchDialog(QWidget * parent)
  : QDialog(parent)
{
  blank = QPixmap(758,1);
  blank.fill();

  setupUi(this);

  manager = new QNetworkAccessManager(this);
  cache = new TvDBCache();

  searchButton->setIcon(QIcon::fromTheme("edit-find"));

  connect(searchButton, SIGNAL(clicked(bool)), this, SLOT(search()));
  connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(requestFinished(QNetworkReply *)));
  connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem  *)), this, SLOT(accept()));
  //connect(listWidget, SIGNAL(itemActivated(QListWidgetItem  *)), this, SLOT(itemSelected(QListWidgetItem  *)));
}

SearchDialog::~SearchDialog()
{
  delete cache;
}

void
SearchDialog::search()
{
  QtTvDB::Mirrors *m = TvDB::mirrors();

  QUrl url = m->searchShowUrl(lineEdit->text());

  QNetworkReply *r = manager->get(QNetworkRequest(url));

  connect(r, SIGNAL(error(QNetworkReply::NetworkError)),
          this, SLOT(error(QNetworkReply::NetworkError)));
}

void
SearchDialog::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
  QNetworkReply *r = dynamic_cast<QNetworkReply *>(sender());

  if (r)
    progressBar->setFormat(r->url().path() + " %p%");
  if (bytesTotal == bytesReceived)
    progressBar->setFormat("%p%");
  progressBar->setRange(0, bytesTotal);
  progressBar->setValue(bytesReceived);
}

void
SearchDialog::error(QNetworkReply::NetworkError code)
{
  QNetworkReply *r = dynamic_cast<QNetworkReply *>(sender());

  if (code == QNetworkReply::OperationCanceledError)
      return;
  if (r)
      QMessageBox::critical(this, tr("Network Error"), r->errorString());
}

void
SearchDialog::requestFinished(QNetworkReply *rep)
{
  QString content = rep->header(QNetworkRequest::ContentTypeHeader).toString();

  if (iconReplies.find(rep) != iconReplies.end()) {
    QPixmap pix;
    QByteArray data = rep->readAll();

    pix.loadFromData(data);
    if (!pix.isNull()) {
      QListWidgetItem *item = iconReplies[rep];

      if (item) {
	item->setIcon(pix);
	cache->storeBannerFile(itemsShows[item]->id(), TvDBCache::Search, data);
	QPixmapCache::insert("search-" + item->text(), pix);
      }
    }
  } else {
    QList < QtTvDB::Show * > shows = QtTvDB::Show::parseShows(rep->readAll());

    if (!shows.size() && !content.startsWith("text/xml"))
      return ;

    clear();

    foreach (QtTvDB::Show *show, shows) {
      QListWidgetItem *item = new QListWidgetItem(show->name(), listWidget);
      QString descr;

      descr = show->overview();
      descr.truncate(64);
      descr.append("...");

      setItemIcon(item, show);
      item->setToolTip(descr);
      listWidget->addItem(item);

      itemsShows[item] = show;
    }
  }
}

void
SearchDialog::clear()
{
    progressBar->setValue(0);

    foreach (QNetworkReply *r, iconReplies.keys()) {
	if (r->isRunning())
	    r->close();
	delete r;
    }
    iconReplies.clear();

    qDeleteAll(itemsShows);
    itemsShows.clear();

    lineEdit->clear();
    listWidget->clear();
}

void
SearchDialog::setItemIcon(QListWidgetItem *item, QtTvDB::Show *show)
{
  QtTvDB::Mirrors *m = TvDB::mirrors();
  QPixmap pix;

  if (QPixmapCache::find("search-" + show->name(), &pix))
    item->setIcon(pix);
  else {
    item->setIcon(blank);

    if (cache->hasBannerFile(show->id(), TvDBCache::Search)) {
      item->setIcon(cache->fetchBannerFile(show->id(), TvDBCache::Search));
    } else if (!show->banner().isEmpty()) {
      QUrl url = m->bannerUrl(show->banner());
      QNetworkReply *r = manager->get(QNetworkRequest(url));

      connect(r, SIGNAL(downloadProgress(qint64, qint64)),
	      this, SLOT(downloadProgress(qint64, qint64)));
      /*
	connect(r, SIGNAL(error(QNetworkReply::NetworkError)),
	      this, SLOT(error(QNetworkReply::NetworkError)));
      */

      iconReplies[r] = item;
    }
  }
}

void
SearchDialog::accept()
{
  hide();
  setResult(QDialog::Accepted);
  emit finished(result());

  QList<QListWidgetItem *> items = listWidget->selectedItems();

  foreach (QListWidgetItem *item, items) {
    QtTvDB::Show *show = itemsShows[item];

    if (show)
      emit showSelected(show->name(), show->id());
  }
  clear();
}

void
SearchDialog::showEvent(QShowEvent * event)
{
    progressBar->setValue(0);
    progressBar->setRange(0, 1);
    lineEdit->setFocus();
    QDialog::showEvent(event);
}

void
SearchDialog::closeEvent(QCloseEvent * event)
{
    clear();
    QDialog::closeEvent(event);
}

