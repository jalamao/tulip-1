//-*-c++-*-
#ifndef MOUSESELECTION_H
#define MOUSESELECTION_H

#include <tulip/InteractorComponent.h>

class QMouseEvent;
class QKeyEvent;

namespace tlp {

class Graph;
/** \addtogroup Mouse_interactor */
/*@{*/
class TLP_QT_SCOPE MouseSelector:public InteractorComponent
{
protected:
  Qt::MouseButton mButton;
  Qt::KeyboardModifier kModifier;
  Qt::KeyboardModifiers mousePressModifier;
  unsigned int x,y;
  int w,h;
  bool started;
  Graph *graph;
public:
  MouseSelector(Qt::MouseButton button = Qt::LeftButton,
		Qt::KeyboardModifier modifier = Qt::NoModifier);
  ~MouseSelector() {}
  bool draw(GlMainWidget *);
  bool eventFilter(QObject *, QEvent *);
  InteractorComponent *clone() { return new MouseSelector(mButton, kModifier); }
};
/*@}*/

}
#endif
