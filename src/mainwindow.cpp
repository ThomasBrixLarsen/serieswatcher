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

#include <QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QtGui/QMessageBox>
#include <QtSql/QSqlDatabase>

#include "config.h"

#include "mainwindow.h"
#include "searchdialog.h"
#include "workerthread.h"
#include "downloadworker.h"
#include "updateprogressdialog.h"
#include "tvdb.h"

MainWindow::MainWindow()
{
  setupUi(this);

  manager = new QNetworkAccessManager(this);
  searchDialog = new SearchDialog(this);

  connect(searchDialog, SIGNAL(showSelected(const QString &, qint64)),
	  this, SLOT(addShow(const QString &, qint64)));

  quitAction->setIcon(QIcon::fromTheme("window-close"));
  updateShowAction->setIcon(QIcon::fromTheme("download"));
  deleteShowAction->setIcon(QIcon::fromTheme("edit-delete"));
  addShowAction->setIcon(QIcon::fromTheme("bookmark-new"));
  settingsAction->setIcon(QIcon::fromTheme("configure"));
  importAction->setIcon(QIcon::fromTheme("document-import"));
  exportAction->setIcon(QIcon::fromTheme("document-export"));
  markWatchedAction->setIcon(QIcon::fromTheme("checkbox"));
  aboutAction->setIcon(QIcon::fromTheme("dialog-information"));

  connect(addShowAction, SIGNAL(triggered()), this, SLOT(addShow()));
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
  connect(aboutQtAction, SIGNAL(triggered()), this, SLOT(aboutQt()));

  QtTvDB::Mirrors *m = TvDB::mirrors();
  m->setKey("FAD75AF31E1B1577");

  thread = new WorkerThread();
  thread->start(QThread::LowPriority);
  progress = new UpdateProgressDialog(this);

  /*
  connect(thread, SIGNAL(downloadStart(qint64, const QUrl &)), progress);
  connect(progress, SIGNAL(abord()), thread, SLOT(abord());

  connect(thread, SIGNAL(databaseUpdated()), this, SLOT(databaseUpdated()));
  connect(thread, SIGNAL(started()), this, SLOT(updateStarted()));
  connect(thread, SIGNAL(finished()), this, SLOT(updateFinished()));
  connect(thread, SIGNAL(error(const QString &, const QString &)),
	  this, SLOT(error(const QString &, const QString &)));
  */
}

MainWindow::~MainWindow()
{
  QSqlDatabase::database("default").close();
  QSqlDatabase::removeDatabase("default");
}

void
MainWindow::addShow()
{
  searchDialog->exec();
}

void
MainWindow::addShow(const QString & name, qint64 id)
{
  progress->show();
  thread->downloadWorker()->updateShow(id);
}

void
MainWindow::about()
{
  QMessageBox::about(this, tr("About Series Watcher " SERIES_WATCHER_VERSION),
		     tr("Version: " SERIES_WATCHER_VERSION "\n"
			"Home: http://xf.iksaif.net/dev/serieswatcher.html\n\n"
			"Copyright (C) 2010 Corentin Chary <corentin.chary@gmail.com>\n"
			"\n"
			"This program is free software; you can redistribute it and/or modify\n"
			"it under the terms of the GNU General Public License as published by\n"
			"the Free Software Foundation; either version 2 of the License, or\n"
			"(at your option) any later version.\n"
			"\n"
			"This program is distributed in the hope that it will be useful, but\n"
			"WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY\n"
			"or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License\n"
			"for more details.\n"
			"\n"
			"You should have received a copy of the GNU General Public License along\n"
			"with this program; if not, write to the Free Software Foundation, Inc.,\n"
			"59 Temple Place, Suite 330, Boston, MA 02111-1307 USA\n"));
}

void
MainWindow::aboutQt()
{
  QMessageBox::aboutQt(this);
}
