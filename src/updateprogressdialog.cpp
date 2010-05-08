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

  icons[Job::Downloading] = QIcon::fromTheme("download");
  icons[Job::Waiting] = QIcon::fromTheme("chronometer");
  icons[Job::Parsing] = QIcon::fromTheme("run-build");
  icons[Job::Failed] = QIcon::fromTheme("dialog-close");
  icons[Job::Finished] = QIcon::fromTheme("dialog-ok");

  connect(detailsButton, SIGNAL(toggled(bool)), this, SLOT(toggleDetails(bool)));

  abording = false;
  listWidget->hide();
  toggleDetails(false);
}


UpdateProgressDialog::~UpdateProgressDialog()
{
}

void
UpdateProgressDialog::toggleDetails(bool clicked)
{
  setMinimumSize(0, 0); /* ? */
  resize(size().width(), sizeHint().height());
}

void
UpdateProgressDialog::updateItem(Job *job)
{
  QString text;

  if (items.find(job) == items.end())
    items[job] = new QListWidgetItem(listWidget);

  if (job->state == Job::Downloading || job->state == Job::Parsing) {
    int perc;

    if (job->done >= job->total)
      perc = 0;
    else
      perc = 100. * job->done / job->total;

    text = QString("%1 (%2%)").arg(job->url.toString()).arg(perc);
  } else
    text = job->url.toString();

  items[job]->setText(text);
  items[job]->setIcon(icons[job->state]);

  if (job->state == Job::Finished || job->state == Job::Failed)
    done[job] = true;
  if (done.size() == jobs.size()) {
    buttonBox->setStandardButtons(QDialogButtonBox::Ok);
    emit finished();
  }
}

void
UpdateProgressDialog::newJob(Job *job)
{
  abording = false;

  if (jobs.size() == 0) {
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel);
    emit started();
  }

  jobs << job;

  updateItem(job);
}

void
UpdateProgressDialog::parseStarted(Job *job)
{
  if (abording)
    return;

  parseLabel->setText(job->url.toString());

  updateItem(job);
}

void
UpdateProgressDialog::parseProgress(Job *job, qint64 done, qint64 total)
{
  if (abording)
    return;

  parseLabel->setText(job->url.toString());
  if (total == -1)
    parseBar->reset();
  parseBar->setValue(done);
  parseBar->setRange(0, total);

  job->done = done;
  job->total = total;

  updateItem(job);
}

void
UpdateProgressDialog::parseFailed(Job *job)
{
  if (abording)
    return;

  updateItem(job);
}

void
UpdateProgressDialog::parseFinished(Job *job)
{
  if (abording)
    return;

  parseLabel->setText("");
  parseBar->reset();
  parseBar->setRange(0, 1);
  parseBar->setValue(1);

  updateItem(job);
}

void
UpdateProgressDialog::downloadStarted(Job *job)
{
  downloadLabel->setText(job->url.toString());

  updateItem(job);
}

void
UpdateProgressDialog::downloadFailed(Job *job, const QString & error)
{
  updateItem(job);
}

void
UpdateProgressDialog::downloadProgress(Job *job, qint64 done, qint64 total)
{
  downloadLabel->setText(job->url.toString());

  if (total == -1)
    downloadBar->reset();
  downloadBar->setValue(done);
  downloadBar->setRange(0, total);

  updateItem(job);
}

void
UpdateProgressDialog::downloadFinished(Job *job)
{
  downloadLabel->setText("");
  downloadBar->reset();
  downloadBar->setRange(0, 1);
  downloadBar->setValue(1);

  updateItem(job);
}

void
UpdateProgressDialog::error(const QString & title, const QString &message)
{
  QMessageBox::critical(this, title, message);
}

void
UpdateProgressDialog::reset()
{
  listWidget->clear();
  items.clear();
  qDeleteAll(jobs);
  jobs.clear();
  done.clear();
  buttonBox->setStandardButtons(QDialogButtonBox::Cancel);
}

void
UpdateProgressDialog::accept()
{
  reset();
  QDialog::accept();
}

void
UpdateProgressDialog::reject()
{
  abording = true;
  emit abord();
  reset();
  QDialog::reject();
}
