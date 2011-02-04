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

#include <QtGui/QDesktopServices>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>

#include "settingsdialog.h"
#include "actiondialog.h"
#include "seriesaction.h"
#include "settings.h"
#include "tvdb.h"
#include "serieswatcher.h"

SettingsDialog::SettingsDialog(QWidget * parent)
  :
#ifdef Q_WS_MAEMO_5
  QDialog(parent, Qt::Window)
#else
  QDialog(parent)
#endif
{
  Settings settings;

  setupUi(this);
  setupDialog(this);

  tabWidget->setTabIcon(0, QIcon::fromTheme("folder-video"));
  tabWidget->setTabIcon(1, QIcon::fromTheme("preferences-other"));
  tabWidget->setTabIcon(2, QIcon::fromTheme("server-database"));

#if defined(Q_WS_MAEMO_5)
  tabWidget->setIconSize(QSize(24, 24));
  databaseGroupBox->setTitle(QString());
  buttonBox->hide();
  cacheGroupBox->setTitle(QString());
#endif

  addButton->setIcon(QIcon::fromTheme("list-add"));
  delButton->setIcon(QIcon::fromTheme("list-remove"));
  editButton->setIcon(QIcon::fromTheme("edit-rename"));

  apiLineEdit->setText(TvDB::mirrors()->key());
  apiLineEdit->setEnabled(false);

  databaseCheckBox->setChecked(settings.value("updateOnStartup").toBool() ? Qt::Checked : Qt::Unchecked);
  prefetchBannersCheckBox->setChecked(settings.value("prefetchBanners").toBool() ? Qt::Checked : Qt::Unchecked);

  connect(clearCacheButton, SIGNAL(clicked()), this, SLOT(clearCache()));
  connect(databaseCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setStartupCheck(int)));
  connect(prefetchBannersCheckBox, SIGNAL(stateChanged(int)), this, SLOT(setPrefetchBanners(int)));
  connect(addButton, SIGNAL(clicked()), this, SLOT(addAction()));
  connect(editButton, SIGNAL(clicked()), this, SLOT(editAction()));
  connect(delButton, SIGNAL(clicked()), this, SLOT(delAction()));
  connect(treeWidget, SIGNAL(itemActivated(QTreeWidgetItem *, int)),
	  this, SLOT(editAction()));
  connect(treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
	  this, SLOT(editAction()));

  populateActions();

  treeWidget->header()->setResizeMode(0, QHeaderView::Stretch);
}

SettingsDialog::~SettingsDialog()
{
}

void
SettingsDialog::setStartupCheck(int state)
{
  Settings settings;

  settings.setValue("updateOnStartup", !!state);
}

void
SettingsDialog::setPrefetchBanners(int state)
{
  Settings settings;

  settings.setValue("prefetchBanners", !!state);
}

bool
SettingsDialog::removeDir(const QString &dirName)
{
  bool result = true;
  QDir dir(dirName);
  QFlags<QDir::Filter> flags = QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files;

  if (!dir.exists())
    return result;

  foreach (QFileInfo info, dir.entryInfoList(flags, QDir::DirsFirst)) {
    if (info.isDir())
      result = removeDir(info.absoluteFilePath());
    else
      result = QFile::remove(info.absoluteFilePath());
    if (!result)
	return result;
    result = dir.rmdir(dirName);
  }
  return result;
}

void
SettingsDialog::clearCache(void)
{
  removeDir(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
}

void
SettingsDialog::addAction()
{
  SeriesAction *action = new SeriesAction();
  ActionDialog dlg(action, this);

  if (dlg.exec())
    createItemFromAction(action);
  else
    delete action;
}

void
SettingsDialog::editAction()
{
  foreach (QTreeWidgetItem *item, treeWidget->selectedItems()) {
    SeriesAction *action = actions[item];

    if (!action)
      continue;

    ActionDialog dlg(action, this);

    if (dlg.exec())
      updateItemForAction(item, action);
  }
}

void
SettingsDialog::delAction()
{
  foreach (QTreeWidgetItem *item, treeWidget->selectedItems()) {
    delete actions[item];
    actions.remove(item);
    delete item;
  }
}

void
SettingsDialog::createItemFromAction(SeriesAction *action)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);

  actions[item] = action;
  updateItemForAction(item, action);
}

void
SettingsDialog::updateItemForAction(QTreeWidgetItem *item, SeriesAction *action)
{
  item->setText(0, action->text());
  item->setIcon(0, action->icon());

  if (!action->showUrl().isEmpty())
    item->setIcon(1, QIcon::fromTheme("checkbox"));
  if (!action->seasonUrl().isEmpty())
    item->setIcon(2, QIcon::fromTheme("checkbox"));
  if (!action->episodeUrl().isEmpty())
    item->setIcon(3, QIcon::fromTheme("checkbox"));

  item->setTextAlignment(1, Qt::AlignHCenter);
  item->setTextAlignment(2, Qt::AlignHCenter);
  item->setTextAlignment(3, Qt::AlignHCenter);
}

void
SettingsDialog::populateActions()
{
  foreach (SeriesAction *action, SeriesAction::fromSettings())
    createItemFromAction(action);
}

void
SettingsDialog::accept()
{
  SeriesAction::addToSettings(actions.values());

  qDeleteAll(actions.values());

  QDialog::accept();
}

void
SettingsDialog::reject()
{
#if defined(Q_WS_MAEMO_5)
  SeriesAction::addToSettings(actions.values());
#endif

  qDeleteAll(actions.values());

  QDialog::reject();
}
