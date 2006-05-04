//-*-c++-*-
#ifndef TULIP_SELECTION_H
#define TULIP_SELECTION_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "AbstractProperty.h"
#include "TemplateFactory.h"
#include "BooleanAlgorithm.h"

class PropertyContext;
/** \addtogroup properties */ 
/*@{*/
class TLP_SCOPE BooleanProperty:public AbstractProperty<BooleanType,BooleanType, BooleanAlgorithm> { 
  friend class BooleanAlgorithm;

public :
  BooleanProperty (Graph *);
  ~BooleanProperty();
  PropertyInterface* clonePrototype(Graph *, std::string );
  void copy( const node, const node, PropertyInterface * );
  void copy( const edge, const edge, PropertyInterface * );
  void reverse();
  ///reverse all the direction of edges of the visible sg which are true in this BooleanProperty
  void reverseEdgeDirection();
  Iterator<node> *getNodesEqualTo(const bool val, Graph * = 0);
  Iterator<edge> *getEdgesEqualTo(const bool val, Graph * = 0);
protected:
  void reset_handler();
};
/*@}*/
#endif
