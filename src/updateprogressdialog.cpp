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

#include "updateprogressdialog.h"
#include "job.h"

UpdateProgressDialog::UpdateProgressDialog(QWidget * parent)
 : QDialog(parent)
{
  setupUi(this);
}

UpdateProgressDialog::~UpdateProgressDialog()
{
}

void
UpdateProgressDialog::newJob(Job *job)
{
  jobs << job;
}

void
UpdateProgressDialog::parseStarted(Job *job)
{
  parseLabel->setText(job->url.toString());
}

void
UpdateProgressDialog::parseProgress(Job *job, qint64 done, qint64 total)
{
  parseLabel->setText(job->url.toString());
  if (total == -1)
    parseBar->reset();
  parseBar->setValue(done);
  parseBar->setRange(0, total);

  job->done = done;
  job->total = total;
}

void
UpdateProgressDialog::parseFailed(Job *job)
{
}

void
UpdateProgressDialog::parseFinished(Job *job)
{
  parseLabel->setText("");
  parseBar->reset();
  parseBar->setRange(0, 1);
  parseBar->setValue(1);
}

void
UpdateProgressDialog::downloadStarted(Job *job)
{
  downloadLabel->setText(job->url.toString());
}

void
UpdateProgressDialog::downloadFailed(Job *job, const QString & error)
{
}

void
UpdateProgressDialog::downloadProgress(Job *job, qint64 done, qint64 total)
{
  downloadLabel->setText(job->url.toString());

  if (total == -1)
    downloadBar->reset();
  downloadBar->setValue(done);
  downloadBar->setRange(0, total);
}

void
UpdateProgressDialog::downloadFinished(Job *job)
{
  downloadLabel->setText("");
  downloadBar->reset();
  downloadBar->setRange(0, 1);
  downloadBar->setValue(1);
}

void
UpdateProgressDialog::error(const QString & title, const QString &message)
{
  QMessageBox::critical(this, title, message);
}
