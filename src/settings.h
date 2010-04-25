#ifndef SETTINGS_H
# define SETTINGS_H

#include <QtCore/QSettings>

class Settings : public QSettings {
  Q_OBJECT
public:
  Settings(QObject * parent = 0);
  ~Settings();
  static Settings *instance();
  static QString path();
private:
  static Settings *instance_;
};

#endif

