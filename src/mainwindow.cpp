#include <QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QtGui/QMessageBox>

#include "mainwindow.h"
#include "searchdialog.h"
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

  QtTvDB::Mirrors *m = TvDB::mirrors();
  m->setKey("FAD75AF31E1B1577");
}

MainWindow::~MainWindow()
{

}

void
MainWindow::addShow()
{
  searchDialog->exec();
}

void
MainWindow::addShow(const QString & name, qint64 id)
{
  QMessageBox::information(this, "test", name);
}
