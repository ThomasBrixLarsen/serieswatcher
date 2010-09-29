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

ShowDialog::ShowDialog(QWidget * parent)
{
  bannerLoader = new BannerLoader(this);
  connect(bannerLoader, SIGNAL(bannerReceived(int)), this, SLOT(bannerReceived()));

  setupUi(this);
  overviewEdit->viewport()->setAutoFillBackground(false);
}

ShowDialog::~ShowDialog()
{
}

void
ShowDialog::setShow(QtTvDB::Show *show, TvDBCache *cache)
{
  QtTvDB::Mirrors *mirrors = TvDB::mirrors();
  QVariantMap map;

  nameLabel->setText(show->name());
  overviewEdit->setText(show->overview());

  map = show->map();
  foreach (QString key, map.keys()) {
    if (map[key].isNull() || map[key].toString().isEmpty())
      continue;
    if (key == "overview" || key == "name" || key == "banner" || key == "poster" || key == "fanArt")
      continue ;

    QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);

    item->setText(0, key);
    item->setText(1, map[key].toString());
    treeWidget->addTopLevelItem(item);
  }

  QString sql;

  sql = "SELECT banners.id as bannerId, banners.path FROM banners ";
  sql += "WHERE (banners.type = 'poster' AND banners.language = 'en') ";
  sql += QString("AND banners.showId = %1").arg(show->id());

  QSqlQuery query(sql);

  if (!query.next())
    return ;

  bannerLabel->setPixmap(QIcon::fromTheme("image-loading").pixmap(160));
  bannerLoader->clear();
  bannerLoader->fetchBanner(0, mirrors->bannerUrl(query.record().value("path").toString()));
}

void
ShowDialog::bannerReceived(void)
{
  bannerLabel->setPixmap(bannerLoader->banner(0).pixmap(QSize(160, 160)));
}
