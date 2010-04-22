#ifndef SEARCHDIALOG_H
# define SEARCHDIALOG_H

#include <QtCore/QMap>

#include "ui_searchdialog.h"

class QNetworkAccessManager;
class QNetworkReply;

class SearchDialog : public QDialog, private Ui_searchDialog
{
  Q_OBJECT
public:
  SearchDialog(QWidget * parent = 0);
  ~SearchDialog();
public slots:
  void search();
  void requestFinished(QNetworkReply *);
private:
  QNetworkAccessManager *manager;
  QMap < QNetworkReply *, QListWidgetItem * > iconReplies;
};

#endif
