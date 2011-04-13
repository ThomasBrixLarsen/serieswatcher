/*
 * Copyright (C) 2010-2011 Corentin Chary <corentin.chary@gmail.com>
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
#include "bannerloader.h"
#include "tvdb.h"
#include "serieswatcher.h"

EpisodeDialog::EpisodeDialog(QWidget * parent)
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

EpisodeDialog::~EpisodeDialog()
{
}

void
EpisodeDialog::setEpisode(QtTvDB::Episode *episode, TvDBCache *cache)
{
  QtTvDB::Mirrors *mirrors = TvDB::mirrors();
  QVariantMap map;
  QString text;

  foreach (QByteArray name, QObject::dynamicPropertyNames())
    map[name] = episode->property(name);

  setWindowTitle(episode->name());

  text += "<p>";
  text += episode->overview();
  text += "</p><p>";

  foreach (QString key, map.keys()) {
    QString title = "";
    QString value = map[key].toString();

    if (key == "firstAired") {
      title = tr("firstAired");
      value = map[key].toDateTime().toString(Qt::SystemLocaleDate);
    } else if (key == "episode")
      title = tr("Episode");
    else if (key == "writer")
      title = tr("Writer");
    else if (key == "guestStars") {
      QStringList list = map[key].toStringList();

      list.removeDuplicates();
      list.removeAll(QString());
      title = tr("Guest Stars");
      value = list.join(", ");
    }

    if (!title.isEmpty() && !value.isEmpty())
      text += QString("<strong>%1:</strong> %2<br />").arg(title).arg(value);
  }
  text += "</p>";

  nameLabel->setText(episode->name());
  overviewEdit->setText(text);


  bannerLabel->setPixmap(QIcon::fromTheme("image-loading").pixmap(400));
  bannerLoader->clear();
  bannerLoader->fetchBanner(0, mirrors->bannerUrl(episode->image()));
#if defined(Q_WS_MAEMO_5)
  QTimer::singleShot(0, this, SLOT(adaptSize()));
#endif
}

void
EpisodeDialog::bannerReceived(void)
{
  QSize size;

  size = QSize(width(), width());

  bannerLabel->show();
  bannerLabel->setPixmap(bannerLoader->banner(0).pixmap(size));
#if defined(Q_WS_MAEMO_5)
  adaptSize();
#else
  resize(QSize(width(), sizeHint().height()));
#endif
}

void
EpisodeDialog::adaptSize(void)
{
#if defined(Q_WS_MAEMO_5)
  QTextDocument *doc = overviewEdit->document();
  QSize size = doc->size().toSize();

  overviewEdit->setMinimumHeight(size.height());
#endif
}
