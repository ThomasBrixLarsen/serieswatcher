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

#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include "seriesaction.h"
#include "settings.h"

Settings::Settings(QObject *parent)
  : QSettings(QSettings::IniFormat, QSettings::UserScope,
	      QCoreApplication::organizationName(),
	      QCoreApplication::applicationName(),
	      parent)
{
}

Settings::~Settings()
{

}

Settings *
Settings::instance()
{
  if (!instance_) {
    instance_ = new Settings();

    if (instance_->value("version").isNull()) {
      QList < SeriesAction * > actions;
      SeriesAction *action = new SeriesAction();

      action = new SeriesAction();
      action->setText(tr("Buy on Amazon"));
      action->setShowUrl(QString("http://www.amazon.com/s/?tag=httpxfiksaifn-21&url=search-alias%3Ddvd&field-keywords=%S"));
      action->setSeasonUrl(QString("http://www.amazon.com/s/?tag=httpxfiksaifn-21&url=search-alias%3Ddvd&field-keywords=%S season %n"));
      action->setIconPath(":/res/amazon.png");
      actions << action;

      SeriesAction::addToSettings(actions);
      qDeleteAll(actions);
    }

    instance_->setValue("version", QCoreApplication::applicationVersion());
    instance_->sync();
  }

  return instance_;
}

QString
Settings::path()
{
  Settings *conf = instance();

  return QFileInfo(conf->fileName()).dir().canonicalPath();
}

Settings *Settings::instance_ = NULL;
