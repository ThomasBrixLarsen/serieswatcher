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
#include <QtGui/QMessageBox>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRecord>
#include <QtCore/QThread>

#include "config.h"

#include "mainwindow.h"
#include "searchdialog.h"
#include "downloadworker.h"
#include "updateworker.h"
#include "updateprogressdialog.h"
#include "tvdb.h"
#include "showdelegate.h"
#include "tvdbcache.h"
#include "settingsdialog.h"
#include "episodedialog.h"
#include "showdialog.h"
#include "settings.h"
#include "tvdbmodel.h"

MainWindow::MainWindow()
{
  setupUi(this);

  setupTvDB();
  createWorkers();
  createActions();
  createSearchDialog();
  setupCache();
  setupModel();
  setupList();
  setupTree();
  displayShows();

  modelsDirty = false;
}

MainWindow::~MainWindow()
{
  updateThread->quit();
  updateThread->wait();

  delete updateWorker;

  QSqlDatabase::database("default").close();
  QSqlDatabase::removeDatabase("default");
}

void
MainWindow::createActions()
{
  homeAction->setIcon(style()->standardIcon(QStyle::SP_DirHomeIcon));
  quitAction->setIcon(QIcon::fromTheme("window-close"));
  updateShowAction->setIcon(QIcon::fromTheme("download"));
  deleteShowAction->setIcon(QIcon::fromTheme("edit-delete"));
  addShowAction->setIcon(QIcon::fromTheme("bookmark-new"));
  importAction->setIcon(QIcon::fromTheme("document-import"));
  exportAction->setIcon(QIcon::fromTheme("document-export"));
  markWatchedAction->setIcon(QIcon::fromTheme("checkbox"));
  markNotWatchedAction->setIcon(QIcon::fromTheme("dialog-cancel"));
  aboutAction->setIcon(QIcon::fromTheme("dialog-information"));
  settingsAction->setIcon(QIcon::fromTheme("preferences-other"));
  aboutQtAction->setIcon(QPixmap(":/trolltech/qmessagebox/images/qtlogo-64.png"));

  connect(homeAction, SIGNAL(triggered()), this, SLOT(displayShows()));
  connect(markWatchedAction, SIGNAL(triggered()), this, SLOT(markWatched()));
  connect(markNotWatchedAction, SIGNAL(triggered()), this, SLOT(markNotWatched()));
  connect(deleteShowAction, SIGNAL(triggered()), this, SLOT(deleteShow()));
  connect(addShowAction, SIGNAL(triggered()), this, SLOT(addShow()));
  connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
  connect(settingsAction, SIGNAL(triggered()), this, SLOT(settings()));
  connect(aboutQtAction, SIGNAL(triggered()), this, SLOT(aboutQt()));
  connect(updateShowAction, SIGNAL(triggered()), this, SLOT(updateShow()));
}

void
MainWindow::reloadActions()
{
  treeView->buildMenus();
  listView->buildMenus();
}

void
MainWindow::setupTvDB()
{
  QtTvDB::Mirrors *m = TvDB::mirrors();
  m->setKey("304DD0AD0616125B");
}

void
MainWindow::createWorkers()
{
  updateThread = new QThread(this);
  progress = new UpdateProgressDialog(this);
  updateBar = new QProgressBar();
  updateButton = new QPushButton(QIcon::fromTheme("download"), tr("Show update progress"));
  updateWorker = new UpdateWorker();
  downloadWorker = new DownloadWorker(this);

  DownloadWorker::setSharedInstance(downloadWorker);

  updateBar->hide();
  updateButton->hide();

  statusBar()->addPermanentWidget(updateBar, 0);
  statusBar()->addPermanentWidget(updateButton, 0);

  connect(updateButton, SIGNAL(clicked(bool)), progress, SLOT(show()));

  connect(this, SIGNAL(destroyed(QObject *)), updateThread, SLOT(quit()));
  connect(updateThread, SIGNAL(started()), this, SLOT(threadStarted()));
  connect(progress, SIGNAL(abort()), downloadWorker, SLOT(abort()));
  connect(progress, SIGNAL(abort()), updateWorker, SLOT(abort()));

  connect(progress, SIGNAL(started()), this, SLOT(updateStarted()));
  connect(progress, SIGNAL(finished()), this, SLOT(updateFinished()));
  connect(progress, SIGNAL(progress(qint64, qint64)), this, SLOT(updateProgress(qint64, qint64)));

  connect(updateWorker, SIGNAL(databaseUpdated()), this, SLOT(databaseUpdated()));
  connect(updateWorker, SIGNAL(newJob(Job *)), downloadWorker, SLOT(startJob(Job *)));
  connect(downloadWorker, SIGNAL(downloadFinished(Job *)), updateWorker, SLOT(startJob(Job *)));

  connect(updateWorker, SIGNAL(newJob(Job *)), progress, SLOT(newJob(Job *)));
  connect(updateWorker, SIGNAL(parseStarted(Job *)), progress, SLOT(parseStarted(Job *)));
  connect(updateWorker, SIGNAL(parseProgress(Job *, qint64, qint64)),
	  progress, SLOT(parseProgress(Job *, qint64, qint64)));
  connect(updateWorker, SIGNAL(parseFailed(Job *)), progress, SLOT(parseFailed(Job *)));
  connect(updateWorker, SIGNAL(parseFinished(Job *)), progress, SLOT(parseFinished(Job *)));
  connect(updateWorker, SIGNAL(error(const QString &, const QString &)),
	  this, SLOT(error(const QString &, const QString &)));

  connect(downloadWorker, SIGNAL(newJob(Job *)), progress, SLOT(newJob(Job *)));
  connect(downloadWorker, SIGNAL(downloadStarted(Job *)), progress, SLOT(downloadStarted(Job *)));
  connect(downloadWorker, SIGNAL(downloadProgress(Job *, qint64, qint64)),
	  progress, SLOT(downloadProgress(Job *, qint64, qint64)));
  connect(downloadWorker, SIGNAL(downloadFailed(Job *, const QString &)),
	  progress, SLOT(downloadFailed(Job *, const QString &)));
  connect(downloadWorker, SIGNAL(downloadFinished(Job *)), progress, SLOT(downloadFinished(Job *)));
  connect(downloadWorker, SIGNAL(error(const QString &, const QString &)),
	  this, SLOT(error(const QString &, const QString &)));

  updateThread->start(QThread::LowPriority);
  updateWorker->moveToThread(updateThread);
}

void
MainWindow::threadStarted()
{
  Settings settings;

  if (settings.value("updateOnStartup").toBool())
    updateShow(-1);
}

void
MainWindow::createSearchDialog()
{
  searchDialog = new SearchDialog(this);

  connect(searchDialog, SIGNAL(showSelected(const QString &, qint64)),
	  this, SLOT(addShow(const QString &, qint64)));
}

void
MainWindow::setupCache()
{
  cache = new TvDBCache();
}

void
MainWindow::setupModel()
{
  tvdbModel = new TvDBModel(this);
}

void
MainWindow::setupList()
{
  //listView->setItemDelegate(new ShowDelegate());

  connect(listView, SIGNAL(clicked(const QModelIndex &)),
	  this, SLOT(itemClicked(const QModelIndex &)));
  connect(listView, SIGNAL(entered(const QModelIndex &)),
	  this, SLOT(itemEntered(const QModelIndex &)));
  connect(listView, SIGNAL(doubleClicked(const QModelIndex &)),
	  this, SLOT(itemDoubleClicked(const QModelIndex &)));

  connectSeriesMenus(listView->getMenus());

  listView->setModel(tvdbModel);

  displayShows();
}

void
MainWindow::setupTree()
{
  connect(treeView, SIGNAL(clicked(const QModelIndex &)),
	  this, SLOT(itemDoubleClicked(const QModelIndex &)));

  connectSeriesMenus(listView->getMenus());

  treeView->setModel(tvdbModel);
}

void
MainWindow::connectSeriesMenus(const SeriesMenus *menus)
{
  connect(menus, SIGNAL(updateShow(qint64)), this, SLOT(updateShow(qint64)));
  connect(menus, SIGNAL(deleteShow(qint64)), this, SLOT(deleteShow(qint64)));
  connect(menus, SIGNAL(episodesWatched(qint64, int, bool)),
	  this, SLOT(episodesWatched(qint64, int, bool)));
  connect(menus, SIGNAL(episodeWatched(qint64, bool)), this, SLOT(episodeWatched(qint64, bool)));
  connect(menus, SIGNAL(episodeDetails(qint64)), this, SLOT(episodeDetails(qint64)));
  connect(menus, SIGNAL(showDetails(qint64)), this, SLOT(showDetails(qint64)));
}

void
MainWindow::itemClicked(const QModelIndex & item)
{
}

void
MainWindow::itemEntered(const QModelIndex & item)
{
}

void
MainWindow::itemDoubleClicked(const QModelIndex & item)
{
  QModelIndex index = tvdbModel->index(item.row(), 0, item.parent());
  int type = index.data(TvDBModel::Type).toInt();

  if (type == TvDBModel::Show)
    displayShow(index);
  else if (type == TvDBModel::Season)
    displaySeason(index);
  else if (type == TvDBModel::Root)
    displayShows();
}

void
MainWindow::displayShows()
{
  listView->setRootIndex(QModelIndex());

  listView->setViewMode(QListView::IconMode);
  listView->setIconSize(QSize(100, 120));
  listView->setGridSize(QSize(150, 150));
}

void
MainWindow::displayShow(const QModelIndex & item)
{
  listView->setRootIndex(item);

  listView->setViewMode(QListView::IconMode);
  listView->setIconSize(QSize(100, 120));
  listView->setGridSize(QSize(150, 150));
}

void
MainWindow::displaySeason(const QModelIndex & item)
{
  listView->setRootIndex(item);

  listView->setViewMode(QListView::ListMode);
  listView->setIconSize(QSize(100, 56));
  listView->setGridSize(QSize(110, 60));
}

void
MainWindow::addShow()
{
  searchDialog->exec();
}

void
MainWindow::settings()
{
  SettingsDialog dlg(this);

  dlg.exec();
  reload();
}

void
MainWindow::addShow(const QString & name, qint64 id)
{
  progress->show();
  downloadWorker->updateShow(id);
}

void
MainWindow::error(const QString & title, const QString & message)
{
  QMessageBox::critical(this, title, message);
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

void
MainWindow::updateShow(qint64 showId)
{
  if (showId >= 0)
    downloadWorker->updateShow(showId);
  else {
    QList < QtTvDB::Show * > shows = cache->fetchShows();

    foreach(QtTvDB::Show *show, shows)
      downloadWorker->updateShow(show->id());

    qDeleteAll(shows);
  }
  //progress->show();
}

void
MainWindow::deleteShow()
{
  QModelIndexList list = listView->selectedIndexes();

  foreach (QModelIndex item, list) {
    int type = item.data(TvDBModel::Id).toInt();

    if (type == TvDBModel::Show)
      cache->deleteShow(item.data(TvDBModel::Id).toLongLong());
  }
  reload();
}

void
MainWindow::deleteShow(qint64 showId)
{
  cache->deleteShow(showId);
  reload();
}

void
MainWindow::episodesWatched(qint64 showId, int season, bool watched)
{
  /* FIXME -> use tvdbModel */
  cache->episodesWatched(showId, season, watched);
}

void
MainWindow::markWatched()
{
  markWatched(true);
}

void
MainWindow::markNotWatched()
{
  markWatched(false);
}

void
MainWindow::markWatched(bool watched)
{
  QModelIndexList list = listView->selectedIndexes();

  foreach (QModelIndex item, list) {
    /*
    QString type = item.data(Qt::UserRole).toString();

    if (type == "show")
      cache->episodesWatched(item.data(ShowModel::Id).toLongLong(), -1, watched);
    if (type == "season")
      cache->episodesWatched(item.data(SeasonModel::Id).toLongLong(),
			     item.data(SeasonModel::Id).toLongLong(), watched);
    if (type == "episode")
      cache->episodeWatched(item.data(EpisodeModel::Id).toLongLong(), watched);
    */
  }
  /* FIXME -> use tvdbModel */
}

void
MainWindow::episodeWatched(qint64 id, bool watched)
{
  cache->episodeWatched(id, watched);
  /* FIXME -> use tvdbModel */
}

void
MainWindow::episodeDetails(qint64 id)
{
  QtTvDB::Episode *episode = cache->fetchEpisode(id);

  if (episode && !episode->isNull()) {
    EpisodeDialog dialog(this);

    dialog.setEpisode(episode, cache);
    dialog.exec();
  }

  delete episode;
}

void
MainWindow::showDetails(qint64 id)
{
  QtTvDB::Show *show = cache->fetchShow(id);

  if (show && !show->isNull()) {
    ShowDialog dialog(this);

    dialog.setShow(show, cache);
    dialog.exec();
  }

  delete show;
}

void
MainWindow::updateStarted()
{
  statusBar()->showMessage(tr("Updating database"));
  updateBar->show();
  updateButton->show();
  //updateBar->resize(QSize(50, updateBar->size().height()));
  updateBar->setRange(0, 1);
  updateBar->setValue(0);
}

void
MainWindow::updateFinished()
{
  updateBar->hide();
  updateButton->hide();
  statusBar()->clearMessage();

  if (modelsDirty) {
    reload();
    modelsDirty = false;
  }
}

void
MainWindow::updateProgress(qint64 done, qint64 total)
{
  updateBar->setRange(0, total);
  updateBar->setValue(done);
}

void
MainWindow::databaseUpdated(void)
{
  modelsDirty = true;
}

void
MainWindow::reload()
{
  tvdbModel->reloadModel();
  reloadActions();
}

