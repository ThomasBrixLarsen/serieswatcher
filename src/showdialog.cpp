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

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>

#include "tvdbcache.h"
#include "showdialog.h"
#include "bannerloader.h"
#include "tvdb.h"
#include "serieswatcher.h"

ShowDialog::ShowDialog(QWidget * parent)
  :
#ifdef Q_WS_MAEMO_5
  QDialog(parent, Qt::Window)
#else
  QDialog(parent)
#endif
{
  bannerLoader = new BannerLoader(this);
  connect(bannerLoader, SIGNAL(bannerReceived(int)), this, SLOT(bannerReceived()));

  setupUi(this);
  setupDialog(this);
#if !defined(Q_WS_MAEMO_5)
  overviewEdit->viewport()->setAutoFillBackground(false);
#endif
}

ShowDialog::~ShowDialog()
{
}

void
ShowDialog::setShow(QtTvDB::Show *show, TvDBCache *cache)
{
  QtTvDB::Mirrors *mirrors = TvDB::mirrors();
  QVariantMap map = show->map();
  QString text;

  setWindowTitle(show->name());

  text += "<p>";
  text += show->overview();
  text += "</p><p>";

  foreach (QString key, map.keys()) {
    QString title = "";
    QString value = map[key].toString();

    if (key == "lastUpdated") {
      title = tr("Last Updated");
      value = map[key].toDateTime().toString(Qt::SystemLocaleDate);
    } else if (key == "airsDay")
      title = tr("Airs Day");
    else if (key == "network")
      title = tr("Network");
    else if (key == "runtime")
      title = tr("Runtime");
    else if (key == "status")
      title = tr("Status");
    else if (key == "actors") {
      QStringList list = map[key].toStringList();

      list.removeDuplicates();
      list.removeAll(QString());
      title = tr("Actors");
      value = list.join(", ");
    }

    if (!title.isEmpty() && !value.isEmpty())
      text += QString("<strong>%1:</strong> %2<br />").arg(title).arg(value);
  }
  text += "</p>";

  nameLabel->setText(show->name());
  overviewEdit->setText(text);

  bannerLabel->setPixmap(QIcon::fromTheme("image-loading").pixmap(160));
  bannerLoader->clear();
  bannerLoader->fetchBanner(0, mirrors->bannerUrl(map["banner"].toString()));
#if defined(Q_WS_MAEMO_5)
  QTimer::singleShot(0, this, SLOT(adaptSize()));
#else
  nameLabel->hide();
#endif
}

void
ShowDialog::bannerReceived(void)
{
  QSize size;

  size = QSize(bannerLabel->width(), 100);

  nameLabel->hide();
  bannerLabel->show();
  bannerLabel->setPixmap(bannerLoader->banner(0).pixmap(size));
#if defined(Q_WS_MAEMO_5)
  adaptSize();
#else
  resize(sizeHint());
#endif
}

void
ShowDialog::adaptSize(void)
{
#if defined(Q_WS_MAEMO_5)
  QTextDocument *doc = overviewEdit->document();
  QSize size = doc->size().toSize();

  overviewEdit->setMinimumHeight(size.height());
#endif
}
