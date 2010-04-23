#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtGui/QPixmapCache>
#include <QtGui/QMessageBox>

#include "searchdialog.h"
#include "tvdb.h"

SearchDialog::SearchDialog(QWidget * parent)
  : QDialog(parent)
{
  blank = QPixmap(758,1);
  blank.fill();

  setupUi(this);

  manager = new QNetworkAccessManager(this);

  searchButton->setIcon(QIcon::fromTheme("edit-find"));

  connect(searchButton, SIGNAL(clicked(bool)), this, SLOT(search()));
  connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(requestFinished(QNetworkReply *)));
  connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem  *)), this, SLOT(accept()));
  //connect(listWidget, SIGNAL(itemActivated(QListWidgetItem  *)), this, SLOT(itemSelected(QListWidgetItem  *)));
}

SearchDialog::~SearchDialog()
{

}

void
SearchDialog::search()
{
  QtTvDB::Mirrors *m = TvDB::mirrors();

  QUrl url = m->searchShowUrl(lineEdit->text());

  manager->get(QNetworkRequest(url));
}

void
SearchDialog::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
  QNetworkReply *r = dynamic_cast<QNetworkReply *>(sender());

  if (r)
    progressBar->setFormat(r->url().path() + " %p%");
  if (bytesTotal == bytesReceived)
    progressBar->setFormat("%p%");
  progressBar->setRange(0, bytesTotal);
  progressBar->setValue(bytesReceived);
}

void
SearchDialog::error(QNetworkReply::NetworkError code)
{
  QNetworkReply *r = dynamic_cast<QNetworkReply *>(sender());

  if (code == QNetworkReply::OperationCanceledError)
      return;
  if (r)
      QMessageBox::critical(this, tr("Network Error"), r->errorString());
}

void
SearchDialog::requestFinished(QNetworkReply *rep)
{
  QString content = rep->header(QNetworkRequest::ContentTypeHeader).toString();

  if (iconReplies.find(rep) != iconReplies.end()) {
    QPixmap pix;

    pix.loadFromData(rep->readAll());
    if (!pix.isNull()) {
      QListWidgetItem *item = iconReplies[rep];

      if (item) {
	item->setIcon(pix);
	QPixmapCache::insert("search-" + item->text(), pix);
      }
    }
  } else {
    QList < QtTvDB::Show * > shows = QtTvDB::Show::parseShows(rep->readAll());

    if (!shows.size() && !content.startsWith("text/xml"))
      return ;

    clear();

    foreach (QtTvDB::Show *show, shows) {
      QListWidgetItem *item = new QListWidgetItem(show->name(), listWidget);
      QString descr;

      descr = show->overview();
      descr.truncate(64);
      descr.append("...");

      setItemIcon(item, show);
      item->setToolTip(descr);
      listWidget->addItem(item);

      itemsShows[item] = show;
    }
  }
}

void
SearchDialog::clear()
{
    progressBar->setValue(0);

    foreach (QNetworkReply *r, iconReplies.keys()) {
	if (r->isRunning())
	    r->close();
	delete r;
    }
    iconReplies.clear();

    qDeleteAll(itemsShows);
    itemsShows.clear();

    lineEdit->clear();
    listWidget->clear();
}

void
SearchDialog::setItemIcon(QListWidgetItem *item, QtTvDB::Show *show)
{
  QtTvDB::Mirrors *m = TvDB::mirrors();
  QPixmap pix;

  if (QPixmapCache::find("search-" + show->name(), &pix))
    item->setIcon(pix);
  else {
    item->setIcon(blank);

    if (!show->banner().isEmpty()) {
      QUrl url = m->bannerUrl(show->banner());
      QNetworkReply *r = manager->get(QNetworkRequest(url));

      connect(r, SIGNAL(downloadProgress(qint64, qint64)),
	      this, SLOT(downloadProgress(qint64, qint64)));
      connect(r, SIGNAL(error(QNetworkReply::NetworkError)),
	      this, SLOT(error(QNetworkReply::NetworkError)));

      iconReplies[r] = item;
    }
  }
}

void
SearchDialog::accept()
{
  hide();
  setResult(QDialog::Accepted);
  emit finished(result());

  QList<QListWidgetItem *> items = listWidget->selectedItems();

  foreach (QListWidgetItem *item, items) {
    QtTvDB::Show *show = itemsShows[item];

    if (show)
      emit showSelected(show->name(), show->id());
  }
  clear();
}

void
SearchDialog::showEvent(QShowEvent * event)
{
    progressBar->setValue(0);
    progressBar->setRange(0, 1);
    lineEdit->setFocus();
    QDialog::showEvent(event);
}

void
SearchDialog::closeEvent(QCloseEvent * event)
{
    clear();
    QDialog::closeEvent(event);
}

