#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtGui/QPixmapCache>

#include "searchdialog.h"
#include "tvdb.h"

SearchDialog::SearchDialog(QWidget * parent)
  : QDialog(parent)
{
  setupUi(this);

  manager = new QNetworkAccessManager(this);

  searchButton->setIcon(QIcon::fromTheme("edit-find"));

  connect(searchButton, SIGNAL(clicked(bool)), this, SLOT(search()));
  connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(requestFinished(QNetworkReply *)));
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
SearchDialog::requestFinished(QNetworkReply *rep)
{
  QtTvDB::Mirrors *m = TvDB::mirrors();
  QByteArray data = rep->readAll();
  QPixmap blank(758,140);

  blank.fill();

  if (iconReplies.find(rep) != iconReplies.end()) {
    QPixmap pix;

    pix.loadFromData(data);
    if (!pix.isNull()) {
      QListWidgetItem *item = iconReplies[rep];

      if (item) {
	item->setIcon(pix);
	QPixmapCache::insert("search-" + item->text(), pix);
      }
    }
  } else {
    QList < QtTvDB::Show * > shows = QtTvDB::Show::parseShows(data);
    QPixmap pix;

    listWidget->clear();

    foreach (QtTvDB::Show *show, shows) {
      QListWidgetItem *item = new QListWidgetItem(show->name(), listWidget);

      if (QPixmapCache::find("search-" + show->name(), &pix))
	item->setIcon(pix);
      else {
	item->setIcon(blank);

	if (!show->banner().isEmpty()) {
	  QUrl url = m->bannerUrl(show->banner());
	  QNetworkReply *r = manager->get(QNetworkRequest(url));

	  iconReplies[r] = item;
	}
      }

      item->setToolTip(show->overview());

      listWidget->addItem(item);
    }
    qDeleteAll(shows);
  }
}
