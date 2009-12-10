#ifndef OUTERPLANARTEST_H
#define OUTERPLANARTEST_H

#include "tulip/tuliphash.h"
#include "tulip/ObservableGraph.h"

namespace tlp {

class Graph;
class edge;
class node;

/** \addtogroup graph_test */ 
/*@{*/
/// class for testing the outerplanarity of a graph
class TLP_SCOPE OuterPlanarTest  : private GraphObserver {
public:
  static bool isOuterPlanar(Graph *graph);
  
private:
  void addEdge(Graph *,const edge);
  void delEdge(Graph *,const edge);
  void reverseEdge(Graph *,const edge);
  void addNode(Graph *,const node);
  void delNode(Graph *,const node);
  void destroy(Graph *);
  bool compute(Graph *graph);
  OuterPlanarTest() : GraphObserver(false) {}
  static OuterPlanarTest* instance;
  TLP_HASH_MAP<unsigned long, bool> resultsBuffer;
};
/*@}*/
}

#endif 
