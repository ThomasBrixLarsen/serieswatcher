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

#include <QtGui/QApplication>
#include <QtGui/QPainter>
#include <QtCore/QDateTime>
#include <QDebug>

#include "showdelegate.h"
#include "showmodel.h"

void
ShowDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
		    const QModelIndex &index) const
{
  //QStyledItemDelegate::paint(painter, option, index);
  //return ;

  if (!index.isValid() || index.column() != 0) {
    QStyledItemDelegate::paint(painter, option, index);
    return;
  }

  QStyleOptionViewItemV4 opt = option;
  initStyleOption(&opt, index);
  const QWidget *widget = opt.widget;
  QStyle *style = widget ? widget->style() : QApplication::style();
  style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, widget);

  QFontMetrics fm(option.font);

  painter->save();

  QFont f = option.font;
  bool unread = index.data().toString() == "Lost";
  QRectF r = option.rect;

  if (unread)
    f.setBold(true);
  painter->setFont(f);
  painter->setPen("#000000");
  painter->drawText(r, Qt::AlignLeft | Qt::AlignTop, index.data().toString());
  painter->restore();

  qDebug() << r;
  painter->save();
  QString status;

  status = QString("%1 Seasons | %2 Episodes (%3 not watched) | Next Airs %4")
    .arg(index.data(ShowModel::Seasons).toInt())
    .arg(index.data(ShowModel::Episodes).toInt())
    .arg(index.data(ShowModel::EpisodesNotWatched).toInt())
    .arg(index.data(ShowModel::NextEpisode).toDateTime().toString());
  qDebug() << status;
  painter->setPen("#7a8288");
  painter->drawText(r, Qt::AlignLeft | Qt::AlignBottom, status);
  painter->restore();
}

QSize
ShowDelegate::sizeHint(const QStyleOptionViewItem &option,
		       const QModelIndex &index) const
{
  QSize size = QStyledItemDelegate::sizeHint(option, index);
  //  qDebug() << "hint" << size;
  //size.setHeight(size.height() * 2);
  return size;
}
