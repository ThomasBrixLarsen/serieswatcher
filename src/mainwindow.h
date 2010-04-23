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

public slots:
  void addShow();
  void addShow(const QString & name, qint64 id);

private:
  SearchDialog *searchDialog;
  QNetworkAccessManager *manager;
};

#endif
