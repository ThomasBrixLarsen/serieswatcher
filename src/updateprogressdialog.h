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

#ifndef UPDATEPROGRESSDIALOG_H
# define UPDATEPROGRESSDIALOG_H

#include <QtGui/QIcon>

#include "tvdb.h"
#include "ui_updateprogressdialog.h"
#include "job.h"

class UpdateProgressModel;

class UpdateProgressDialog : public QDialog, private Ui_UpdateProgressDialog
{
  Q_OBJECT
public:
  UpdateProgressDialog(QWidget * parent = 0);
  ~UpdateProgressDialog();

  bool isWorking();

public slots:
  void newJob(Job *job);

  void parseStarted(Job *job);
  void parseProgress(Job *job, qint64 done, qint64 total);
  void parseFailed(Job *job);
  void parseFinished(Job *job);

  void downloadStarted(Job *job);
  void downloadFailed(Job *job, const QString & error);
  void downloadProgress(Job *job, qint64 done, qint64 total);
  void downloadFinished(Job *job);

  void error(const QString & title, const QString &message);

  void toggleDetails(bool clicked);

  void accept();
  void reject();

signals:
  void abort();
  void finished();
  void started();
  void progress(qint64 done, qint64 total);

private:
  void updateItem(Job *job);
  void reset();

private:
  QList < Job * > jobs;
  QMap < Job *, QListWidgetItem * > items;
  QMap < Job::State, QIcon > icons;
  QMap < Job *, bool > done;
  bool aborting;
  bool working;
};

#endif
