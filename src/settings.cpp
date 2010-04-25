#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

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
