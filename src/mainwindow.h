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

#ifndef MAINWINDOW_H
# define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtCore/QStack>

#include "ui_mainwindow.h"

class ListWindow;
class SearchDialog;
class DownloadWorker;
class UpdateWorker;
class UpdateProgressDialog;
class TvDBModel;
class TvDBCache;
class QTreeWidgetItem;
class QProgressBar;
class QPushButton;

class MainWindow : public QMainWindow, private Ui_mainWindow {
  Q_OBJECT
public:
  MainWindow();
  ~MainWindow();

private slots:
  void addShow();
  void addShow(const QString & name, qint64 id);

  void about();
  void settings();
  void aboutQt();
  void error(const QString & title, const QString & message);

  void threadStarted();

  void itemOpened(const QModelIndex & index, bool dclick);
  void itemClicked(const QModelIndex & index);
  void itemEntered(const QModelIndex & index);
  void itemDoubleClicked(const QModelIndex & index);

  void updateShow(qint64 showId = -1);
  void deleteShow();
  void deleteShow(qint64 showId);

  void markWatched();
  void markNotWatched();
  void markWatched(bool watched);
  void episodesWatched(const QModelIndex & index, bool watched);
  void episodeDetails(qint64 id);
  void showDetails(qint64 id);

  void updateStarted();
  void updateFinished();
  void updateProgress(qint64 done, qint64 total);
  void databaseUpdated();
  void reload();

  void displayShows();
  void displayShow(const QModelIndex &item);
  void displaySeason(const QModelIndex &item);

  void windowClosed();

private:
  void setupTvDB();
  void createWorkers();
  void createActions();
  void reloadActions();
  void createSearchDialog();
  void createSettingsDialog();
  void connectSeriesMenus(const SeriesMenus *menus);
  void setupCache();
  void setupModel();
  void setupList(MainListView *view);
  void setupTree();

  MainListView *newView(const QModelIndex &item);
  MainListView *topView(void);
  QWidget *parentWidget(void);

private:
  int currentShowId;
  int currentSeason;
  int currentEpisodeId;

  bool modelsDirty;

  QThread *updateThread;
  DownloadWorker *downloadWorker;
  UpdateWorker *updateWorker;

  UpdateProgressDialog *progress;
  QProgressBar *updateBar;
  QPushButton *updateButton;
  SearchDialog *searchDialog;
  TvDBModel *tvdbModel;
  TvDBCache *cache;

  /* Used for Maemo 5 */
  QStack < ListWindow * > childWindows;
};

#endif
