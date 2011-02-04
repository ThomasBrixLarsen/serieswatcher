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

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtCore/QFileInfo>

#include "actiondialog.h"
#include "seriesaction.h"

ActionDialog::ActionDialog(SeriesAction *a, QWidget * parent)
  : QDialog(parent)
{
  action = a;

  setupUi(this);

  load();
}

void
ActionDialog::load()
{
  iconLineEdit->setText(action->iconPath());
  iconPixLabel->setPixmap(QPixmap(action->iconPath()).
			  scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));

  actionLineEdit->setText(action->text());

  if (!action->showUrl().isEmpty()) {
    showCheckBox->setChecked(true);
    showLineEdit->setText(action->showUrl().toString());
  }

  if (!action->seasonUrl().isEmpty()) {
    seasonCheckBox->setChecked(true);
    seasonLineEdit->setText(action->seasonUrl().toString());
  }

  if (!action->episodeUrl().isEmpty()) {
    episodeCheckBox->setChecked(true);
    episodeLineEdit->setText(action->episodeUrl().toString());
  }

  connect(toolButton, SIGNAL(clicked()), this, SLOT(selectIcon()));
}

ActionDialog::~ActionDialog()
{
}

void
ActionDialog::selectIcon()
{
  QString path = QFileDialog::getOpenFileName(this, tr("Select an icon"),
					      QFileInfo(action->iconPath()).dir().absolutePath(),
					      tr("Images (*.png *.xpm *.jpg)"));

  if (path.isEmpty())
    return ;

  QPixmap pix(path);

  if (pix.isNull())
    QMessageBox::warning(this, tr("Error"), tr("Invalid image file."));

  iconLineEdit->setText(path);
  iconPixLabel->setPixmap(pix.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void
ActionDialog::accept()
{
  action->setIconPath(iconLineEdit->text());
  action->setText(actionLineEdit->text());

  action->setShowUrl(QUrl());
  action->setSeasonUrl(QUrl());
  action->setEpisodeUrl(QUrl());

  if (showCheckBox->isChecked())
    action->setShowUrl(QUrl::fromUserInput(showLineEdit->text()));
  if (seasonCheckBox->isChecked())
    action->setSeasonUrl(QUrl::fromUserInput(seasonLineEdit->text()));
  if (episodeCheckBox->isChecked())
    action->setEpisodeUrl(QUrl::fromUserInput(episodeLineEdit->text()));

  QDialog::accept();
}

