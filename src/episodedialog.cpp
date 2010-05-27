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

#include "episodedialog.h"
#include "tvdbcache.h"

EpisodeDialog::EpisodeDialog(QWidget * parent)
{
  setupUi(this);
  overviewEdit->viewport()->setAutoFillBackground(false);
}

EpisodeDialog::~EpisodeDialog()
{
}

void
EpisodeDialog::setEpisode(QtTvDB::Episode *episode, TvDBCache *cache)
{
  QVariantMap map;

  nameLabel->setText(episode->name());
  overviewEdit->setText(episode->overview());

  map = episode->map();
  foreach (QString key, map.keys()) {
    if (map[key].isNull() || map[key].toString().isEmpty())
      continue;
    if (key == "overview" || key == "name" || key == "image")
      continue ;

    QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);

    item->setText(0, key);
    item->setText(1, map[key].toString());
    treeWidget->addTopLevelItem(item);
  }

  QPixmap pixmap = cache->fetchBannerFile(episode->id(), TvDBCache::Episode, QSize(100, 120));
  bannerLabel->setPixmap(pixmap);
}

