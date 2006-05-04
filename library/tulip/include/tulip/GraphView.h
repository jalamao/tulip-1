//-*-c++-*-

/*
 Author: David Auber
 Email : auber@labri.fr
 Last modification : 20/08/2001
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/

#ifndef Tulip_SUPERGRAPHVIEW_H
#define Tulip_SUPERGRAPHVIEW_H

#ifndef DOXYGEN_NOTFOR_USER

#include "tulip/GraphAbstract.h"
#include "tulip/MutableContainer.h"


/**
 * This class is one the implementation of the Graph Interface
 * It only filters the elements of its parents.
 */
class GraphView:public GraphAbstract {
    
public:
  GraphView(Graph *father,BooleanProperty *filter);
  ~GraphView();
  //========================================================================
  node addNode();
  edge addEdge(const node n1,const node n2);
  void addNode(const node);
  void addEdge(const edge);
  void delNode(const node);
  void delEdge(const edge);
  void delAllNode(const node);
  void delAllEdge(const edge);
  void setEdgeOrder(const node,const std::vector<edge> & );
  void swapEdgeOrder(const node,const edge , const edge );
  //  void reverse(const edge);
  //=========================================================================
  bool isElement(const node ) const;
  bool isElement(const edge ) const;
  unsigned int numberOfNodes() const;
  unsigned int numberOfEdges() const;
    //=========================================================================
  /*  unsigned int deg(const node) const;
  unsigned int indeg(const node) const;
  unsigned int outdeg(const node) const;*/
  //=========================================================================
  Iterator<node>* getNodes() const;
  Iterator<node>* getInNodes(const node) const;
  Iterator<node>* getOutNodes(const node) const;
  Iterator<node>* getInOutNodes(const node) const;
  Iterator<edge>* getEdges() const;
  Iterator<edge>* getInEdges(const node) const;
  Iterator<edge>* getOutEdges(const node) const;
  Iterator<edge>* getInOutEdges(const node) const;
  //=========================================================================

private:
  MutableContainer<bool> nodeAdaptativeFilter;
  MutableContainer<bool> edgeAdaptativeFilter;
  //  MutableContainer<unsigned int> outDegree;
  //  MutableContainer<unsigned int> inDegree;
  mutable int nNodes;
  mutable int nEdges;
};


#endif

#endif



