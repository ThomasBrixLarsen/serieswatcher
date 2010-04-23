#include "searchlistwidget.h"

SearchListWidget::SearchListWidget(QWidget * parent)
  : QListWidget(parent)
{
}

SearchListWidget::~SearchListWidget()
{
}

QStyleOptionViewItem
SearchListWidget::viewOptions() const
{
  QStyleOptionViewItem option = QListWidget::viewOptions();

  if (viewMode() == QListView::ListMode)
    option.decorationPosition = QStyleOptionViewItem::Right;
  return option;
}
