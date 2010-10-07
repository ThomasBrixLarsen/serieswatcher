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

#ifndef SETTINGSDIALOG_H
# define SETTINGSDIALOG_H


#include <QtGui/QDialog>

#include "ui_settingsdialog.h"

class SeriesAction;
class QTreeWidgetItem;

class SettingsDialog : public QDialog, private Ui_SettingsDialog
{
  Q_OBJECT
public:
  SettingsDialog(QWidget * parent = 0);
  ~SettingsDialog();

public slots:
  void accept();

private slots:
  void addAction();
  void editAction();
  void delAction();
  void setStartupCheck(int state);
  void setPrefetchBanners(int state);
  void clearCache();
  bool removeDir(const QString & dirname);

private:
  void populateActions();
  void createItemFromAction(SeriesAction *action);
  void updateItemForAction(QTreeWidgetItem *item, SeriesAction *action);

  QMap < QTreeWidgetItem * , SeriesAction * > actions;
};

#endif
