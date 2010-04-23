#ifndef SEARCHDIALOG_H
# define SEARCHDIALOG_H

#include <QtCore/QMap>
#include <QtNetwork/QNetworkReply>

#include "tvdb.h"
#include "ui_searchdialog.h"

class QNetworkAccessManager;

class SearchDialog : public QDialog, private Ui_searchDialog
{
  Q_OBJECT
public:
  SearchDialog(QWidget * parent = 0);
  ~SearchDialog();
signals:
  void showSelected(const QString & name, qint64 id);

public slots:
  void accept();

private slots:
  void search();
  void requestFinished(QNetworkReply *rep);
  void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
  void error(QNetworkReply::NetworkError code);

private:
  void clear();
  void setItemIcon(QListWidgetItem *item, QtTvDB::Show *show);
  QNetworkAccessManager *manager;
  QMap < QNetworkReply *, QListWidgetItem * > iconReplies;
  QMap < QListWidgetItem *, QtTvDB::Show * > itemsShows;
  QPixmap blank;
};

#endif
