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

#if defined(Q_WS_MAEMO_5)
#include <QtGui/QAbstractKineticScroller>
#endif

#include "serieswatcher.h"
#include "aboutdialog.h"

#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent)
  :
#ifdef Q_WS_MAEMO_5
  QDialog(parent, Qt::Window)
#else
  QDialog(parent)
#endif
{
  ui = new Ui_AboutDialog();
  ui->setupUi(this);
  setupDialog(this);
  ui->versionLabel->setText(ui->versionLabel->text() + QString(" %1").arg(SERIES_WATCHER_VERSION));

#if defined(Q_WS_MAEMO_5)
  ui->buttonBox->hide();

  QAbstractKineticScroller *scroller;

  if ((scroller = ui->licenceBrowser->property("kineticScroller").value<QAbstractKineticScroller *>()) != NULL)
    scroller->setEnabled(true);

  if ((scroller = ui->authorsBrowser->property("kineticScroller").value<QAbstractKineticScroller *>()) != NULL)
    scroller->setEnabled(true);
#endif
}

AboutDialog::~AboutDialog()
{
  delete ui;
}
