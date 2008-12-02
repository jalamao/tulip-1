//-*-c++-*-
/**
 Author: David Auber
 Email : auber@labri.fr
 Last modification : 01/09/2003
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/

#ifndef TULIP_CONNECTEDTEST_H
#define TULIP_CONNECTEDTEST_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <set>
#include "tulip/ObservableGraph.h"

namespace tlp {

class Graph;

/** \addtogroup graph_test */ 
/*@{*/
/// class for testing if the graph is connected
class TLP_SCOPE ConnectedTest : private GraphObserver {
public:
  /**
   * Returns true if the graph is connected (ie. it exists an undirected path 
   * between each pair of nodes) else false.
   */
  static bool isConnected(Graph *graph);
  /**
   * If the graph is not connected, adds edges in order to make the graph
   * connected. The new edges are added in addedEdges.
   */
  static void makeConnected(Graph *graph, std::vector<edge>& addedEdges);
  /**
   * Returns the number of connected components in the graph;
   */
  static unsigned int numberOfConnectedComponents(Graph *graph);
  /**
   * Compute the nodes for each connected component
   */
  static void computeConnectedComponents(Graph *graph, std::vector< std::set<node> >& components);
    
private:
  void connect(Graph *, std::vector<node>& toLink);
  bool compute(Graph *);
  void addEdge(Graph *,const edge);
  void delEdge(Graph *,const edge);
  void reverseEdge(Graph *,const edge);
  void addNode(Graph *,const node);
  void delNode(Graph *,const node);
  void destroy(Graph *);
  ConnectedTest();
  static ConnectedTest * instance;
  stdext::hash_map<unsigned long,bool> resultsBuffer;
};

}
/*@}*/
#endif
