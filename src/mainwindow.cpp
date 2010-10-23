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
#include <QtCore/QTimer>

#include "config.h"

#include "mainwindow.h"
#include "listwindow.h"
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
#include "aboutdialog.h"

MainWindow::MainWindow()
{
  setupUi(this);

  setupTvDB();
  createWorkers();
  createActions();
  createSearchDialog();
  setupCache();
  setupModel();
#if !defined(Q_WS_MAEMO_5)
  setupTree();
#endif

  modelsDirty = false;
#if defined(Q_WS_MAEMO_5)
  setAttribute(Qt::WA_Maemo5StackedWindow);
  setAttribute(Qt::WA_Maemo5AutoOrientation, true);

  treeDock->hide();
  toolBar->hide();
  statusBar()->hide();

  menu_Edit->removeAction(markWatchedAction);
  menu_Edit->removeAction(markNotWatchedAction);
  menu_Edit->removeAction(deleteShowAction);
  menu_Help->removeAction(aboutQtAction);
  menu_File->removeAction(importAction);
  menu_File->removeAction(exportAction);
#endif
  QTimer::singleShot(100, this, SLOT(delayedInit()));
}

MainWindow::~MainWindow()
{
  updateThread->quit();
  updateThread->wait();

  delete updateWorker;

  QSqlDatabase::database("default").close();
  QSqlDatabase::removeDatabase("default");

  delete cache;
}

void
MainWindow::delayedInit()
{
  setupList(listView);
  displayShows();
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
  if (treeView)
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
  cache = TvDBCache::instance();
}

void
MainWindow::setupModel()
{
  tvdbModel = new TvDBModel(this);
}

void
MainWindow::setupList(MainListView *view)
{
  //view->setItemDelegate(new ShowDelegate());

  connect(view, SIGNAL(clicked(const QModelIndex &)),
	  this, SLOT(itemClicked(const QModelIndex &)));
  connect(view, SIGNAL(entered(const QModelIndex &)),
	  this, SLOT(itemEntered(const QModelIndex &)));
  connect(view, SIGNAL(doubleClicked(const QModelIndex &)),
	  this, SLOT(itemDoubleClicked(const QModelIndex &)));

  connectSeriesMenus(view->getMenus());

  view->setModel(tvdbModel);
  view->setModelColumn(7);
}

void
MainWindow::setupTree()
{
  connect(treeView, SIGNAL(clicked(const QModelIndex &)),
	  this, SLOT(itemDoubleClicked(const QModelIndex &)));

  connectSeriesMenus(treeView->getMenus());

  treeView->setModel(tvdbModel);
}

void
MainWindow::connectSeriesMenus(const SeriesMenus *menus)
{
  connect(menus, SIGNAL(updateShow(qint64)), this, SLOT(updateShow(qint64)));
  connect(menus, SIGNAL(deleteShow(qint64)), this, SLOT(deleteShow(qint64)));
  connect(menus, SIGNAL(episodesWatched(const QModelIndex &, bool)),
	  this, SLOT(episodesWatched(const QModelIndex &, bool)));
  connect(menus, SIGNAL(episodeDetails(qint64)), this, SLOT(episodeDetails(qint64)));
  connect(menus, SIGNAL(showDetails(qint64)), this, SLOT(showDetails(qint64)));
}

void
MainWindow::itemClicked(const QModelIndex & item)
{
  itemOpened(item, false);
}

void
MainWindow::itemEntered(const QModelIndex & item)
{

}

void
MainWindow::itemDoubleClicked(const QModelIndex & item)
{
  itemOpened(item, true);
}

void
MainWindow::itemOpened(const QModelIndex & item, bool dclick)
{
#if !defined(Q_WS_MAEMO_5)
  if (!dclick) /* Only act on double click for desktop */ 
    return ;
#endif

  QModelIndex index = tvdbModel->index(item.row(), 0, item.parent());
  int type = index.data(TvDBModel::Type).toInt();

  if (type == TvDBModel::Show)
    displayShow(index);
  else if (type == TvDBModel::Season)
    displaySeason(index);
  else if (type == TvDBModel::Episode) {
#if defined(Q_WS_MAEMO_5)
    if (dclick) /* Because click may be for the checkbox ..*/
      episodeDetails(item.data(TvDBModel::Id).toInt());
#else
  episodeDetails(item.data(TvDBModel::Id).toInt());
#endif
  } else if (type == TvDBModel::Root)
    displayShows();
}

void
MainWindow::displayShows()
{
  MainListView *view = topView();

  view->setViewMode(QListView::IconMode);

#if defined(Q_WS_MAEMO_5)
  view->setIconSize(QSize(204, 300));
  view->setGridSize(QSize(240, 320));
#else
  view->setIconSize(QSize(100, 120));
  view->setGridSize(QSize(150, 150));
#endif

  view->setRootIndex(QModelIndex());
}

void
MainWindow::displayShow(const QModelIndex & item)
{
  MainListView *view = newView(item);

  view->setViewMode(QListView::IconMode);
#if defined(Q_WS_MAEMO_5)
  view->setIconSize(QSize(204, 300));
  view->setGridSize(QSize(240, 320));
#else
  view->setIconSize(QSize(100, 120));
  view->setGridSize(QSize(150, 150));
#endif

  view->setRootIndex(item);
}

void
MainWindow::displaySeason(const QModelIndex & item)
{
  MainListView *view = newView(item);

  view->setViewMode(QListView::ListMode);
#if defined(Q_WS_MAEMO_5)
  view->setIconSize(QSize(115, 60));
  view->setUniformItemSizes(true);
#else
  view->setIconSize(QSize(100, 56));
  view->setGridSize(QSize(110, 60));
#endif

  view->setRootIndex(item);
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
  AboutDialog dialog(this);

  dialog.exec();
}

void
MainWindow::aboutQt()
{
  QMessageBox::aboutQt(this);
}

void
MainWindow::updateShow(qint64 showId)
{
#if defined(Q_WS_MAEMO_5)
  progress->show();
#endif

  if (showId >= 0)
    downloadWorker->updateShow(showId);
  else {
    QList < QtTvDB::Show * > shows = cache->fetchShows();

    foreach(QtTvDB::Show *show, shows)
      downloadWorker->updateShow(show->id());

    qDeleteAll(shows);
  }
}

void
MainWindow::deleteShow()
{
  MainListView *view = topView();
  QModelIndexList list = view->selectedIndexes();

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
MainWindow::episodesWatched(const QModelIndex & index, bool watched)
{
  tvdbModel->setData(index, watched, TvDBModel::EpisodesWatched);
}

void
MainWindow::stackMarkWatched()
{
  stackMarkWatched(true);
}

void
MainWindow::stackMarkNotWatched()
{
  stackMarkWatched(false);
}

void
MainWindow::stackMarkWatched(bool watched)
{
  MainListView *view = topView();
  QModelIndex root = view->rootIndex();

  if (root.isValid())
    tvdbModel->setData(root, watched, TvDBModel::EpisodesWatched);
}

void
MainWindow::stackInfos()
{
  MainListView *view = topView();
  QModelIndex root = view->rootIndex();

  if (!root.isValid())
    return ;

  if (root.data(TvDBModel::Type) == TvDBModel::Season)
    root = root.parent();

  if (root.data(TvDBModel::Type) == TvDBModel::Show)
    showDetails(root.data(TvDBModel::Id).toLongLong());
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
  MainListView *view = topView();
  QModelIndexList list = view->selectedIndexes();

  foreach (QModelIndex item, list)
    tvdbModel->setData(item, watched, TvDBModel::EpisodesWatched);
}

void
MainWindow::episodeDetails(qint64 id)
{
  QtTvDB::Episode *episode = cache->fetchEpisode(id);

  if (episode && !episode->isNull()) {
    EpisodeDialog dialog(parentWidget());

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
    ShowDialog dialog(parentWidget());

    dialog.setShow(show, cache);
    dialog.exec();
  }

  delete show;
}

void
MainWindow::updateStarted()
{
#if defined(Q_WS_MAEMO_5)
  foreach (ListWindow *window, childWindows)
    window->setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
  setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
#endif
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
#if defined(Q_WS_MAEMO_5)
  foreach (ListWindow *window, childWindows)
    window->setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
  setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
  progress->hide();
#endif
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
  foreach (ListWindow *window, childWindows)
    window->close();

  listView->setModel(NULL);

  if (treeView)
    treeView->setModel(NULL);

  delete tvdbModel;
  tvdbModel = new TvDBModel(this);

  listView->setModel(tvdbModel);
  listView->setModelColumn(7);

  if (treeView)
    treeView->setModel(tvdbModel);

  displayShows();

  reloadActions();
}

/* Stacked windows hack for Maemo 5 */
MainListView *
MainWindow::newView(const QModelIndex & item)
{
#if !defined(Q_WS_MAEMO_5)
  return listView;
#else
  ListWindow *window;
  MainListView *view;

  window = new ListWindow(parentWidget());
  view = window->view();

  view->buildMenus();
  setupList(view);

  connect(window, SIGNAL(destroyed(QObject *)), this, SLOT(windowClosed()));

  childWindows.push(window);

  window->setWindowTitle(item.data(Qt::DisplayRole).toString());
  window->setAttribute(Qt::WA_DeleteOnClose, true);
  window->show();

  connect(window, SIGNAL(markWatched()), this, SLOT(stackMarkWatched()));
  connect(window, SIGNAL(markNotWatched()), this, SLOT(stackMarkNotWatched()));
  connect(window, SIGNAL(infos()), this, SLOT(stackInfos()));
  return view;
#endif
}

QWidget *
MainWindow::parentWidget()
{
  if (!childWindows.isEmpty())
    return childWindows.top();
  return this;
}

MainListView *
MainWindow::topView()
{
  if (!childWindows.isEmpty())
    return childWindows.top()->view();
  return listView;
}

void
MainWindow::windowClosed()
{
  childWindows.pop();
}
