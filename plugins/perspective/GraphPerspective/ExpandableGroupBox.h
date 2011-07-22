#ifndef EXPANDABLEGROUPBOX_H
#define EXPANDABLEGROUPBOX_H

#include <QtGui/QGroupBox>

class ExpandableGroupBox : public QGroupBox {
  Q_OBJECT

  Q_PROPERTY(bool expanded READ expanded WRITE setExpanded)
  bool _expanded;

  QMargins _oldMargins;
public:
  explicit ExpandableGroupBox(const QString &title=QString(),QWidget *parent = 0);
  virtual ~ExpandableGroupBox();
  bool expanded() const {
    return _expanded;
  }

signals:

public slots:
  void setExpanded(bool e);

protected slots:
  void layoutDestroyed();
};

#endif // EXPANDABLEGROUPBOX_H
