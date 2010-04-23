#ifndef MAINWINDOW_H
# define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include "ui_mainwindow.h"

class SearchDialog;
class QNetworkAccessManager;

class MainWindow : public QMainWindow, private Ui_mainWindow {
  Q_OBJECT
public:
  MainWindow();
  ~MainWindow();

private slots:
  void addShow();
  void addShow(const QString & name, qint64 id);
  void about();
  void aboutQt();

private:
  SearchDialog *searchDialog;
  QNetworkAccessManager *manager;
};

#endif
