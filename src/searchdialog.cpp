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

#include <QtGui/QMessageBox>

#include "searchdialog.h"
#include "searchmodel.h"
#include "tvdb.h"
#include "serieswatcher.h"

SearchDialog::SearchDialog(QWidget * parent)
  :
#ifdef Q_WS_MAEMO_5
  QDialog(parent, Qt::Window)
#else
  QDialog(parent)
#endif
{
  setupUi(this);
  setupDialog(this);

#if defined(Q_WS_MAEMO_5)
  progressBar->hide();
  buttonBox->hide();
#endif

  model = new SearchModel(this);
  connect(model, SIGNAL(searchProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));
  connect(model, SIGNAL(networkError(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
  listView->setModel(model);

  searchButton->setIcon(QIcon::fromTheme("edit-find"));

  connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(search()));
  connect(searchButton, SIGNAL(clicked(bool)), this, SLOT(search()));

#ifdef Q_WS_MAEMO_5
  connect(listView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(accept()));
#else
  connect(listView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(accept()));
#endif
}

SearchDialog::~SearchDialog()
{
}

void
SearchDialog::search()
{
  // FIXME check QtBearer
  model->setQuery(lineEdit->text());
}

void
SearchDialog::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
  if (bytesTotal == -1)
    bytesTotal = bytesReceived;

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
SearchDialog::clear()
{
  progressBar->setValue(0);
  model->setQuery(QString());
  lineEdit->clear();
}

void
SearchDialog::accept()
{
  hide();
  setResult(QDialog::Accepted);
  emit finished(result());

  foreach (QModelIndex index, listView->selectedIndexes()) {
    QtTvDB::Show *show = (QtTvDB::Show *)index.data(SearchModel::ShowRole).value<void *>();

    if (show)
      emit showSelected(show->name(), show->id());
  }
  clear();
}

void
SearchDialog::showEvent(QShowEvent * event)
{
    clear();
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
