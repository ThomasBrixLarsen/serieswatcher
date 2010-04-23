#ifndef SEARCH_LIST_WIDGET_H
# define SEARCH_LIST_WIDGET_H

#include <QtGui/QListWidget>

class SearchListWidget : public QListWidget
{
  Q_OBJECT
public:
  SearchListWidget(QWidget * parent = 0);
  ~SearchListWidget();
protected:
  QStyleOptionViewItem viewOptions() const;
};

#endif
