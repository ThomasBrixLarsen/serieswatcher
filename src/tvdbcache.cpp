#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtCore/QDir>

#include "settings.h"
#include "tvdbcache.h"

#include "schema.h"

TvDBCache::TvDBCache()
{
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

  db.setDatabaseName(Settings::path() + QDir::separator() + "serieswatcher.db");
  db.open();
  QStringList queries = QString(schema).split(";");

  foreach(QString query, queries) {
    QSqlQuery q(query);

    if (!q.exec()) {
      qWarning() << q.lastError();
      qWarning() << query;
    }
  }
}

TvDBCache::~TvDBCache()
{
}

void
TvDBCache::storeShow(QtTvDB::Show *show)
{
}

void
TvDBCache::storeEpisode(QtTvDB::Episode *episode)
{
}

void
TvDBCache::storeEpisodes(QList < QtTvDB::Episode * > episodes)
{
}

void
TvDBCache::storeBanner(QtTvDB::Banner *banner)
{
}

void
TvDBCache::storeBanners(QList < QtTvDB::Banner * > banners)
{
}

QtTvDB::Show *
TvDBCache::fetchShow()
{
  return NULL;
}

QtTvDB::Episode *
TvDBCache::fetchEpisode()
{
  return NULL;
}

QList < QtTvDB::Episode * >
TvDBCache::fetchEpisodes(qint64 showId, qint64 seasonId)
{
  return QList < QtTvDB::Episode * >();
}

QtTvDB::Banner *
TvDBCache::fetchBanner(qint64 id)
{
  return NULL;
}

QList < QtTvDB::Banner * >
TvDBCache::fetchBanners(qint64 showId)
{
  return QList < QtTvDB::Banner * >();
}
