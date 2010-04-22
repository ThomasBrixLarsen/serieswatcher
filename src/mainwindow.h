#ifndef MAINWINDOW_H
# define MAINWINDOW_H

#include <QtGui/QMainWindow>

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui_mainWindow {
  Q_OBJECT
public:
  MainWindow();
  ~MainWindow();
};

#endif
