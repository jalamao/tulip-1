/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux 1 and Inria Bordeaux - Sud Ouest
 *
 * Tulip is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * Tulip is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 */
#include "LeafMetric.h"

DOUBLEPLUGINOFGROUP(LeafMetric,"Leaf","David Auber","20/12/1999","Alpha","1.0","Tree");

using namespace std;
using namespace tlp;

//=======================================================================
LeafMetric::LeafMetric(const tlp::PropertyContext &context):DoubleAlgorithm(context) {}
//=======================================================================
double LeafMetric::getNodeValue(const tlp::node n) {
  if (result->getNodeValue(n) != 0) 
    return result->getNodeValue(n);
  double res=0;
  node _n;
  forEach(_n, graph->getOutNodes(n)) 
    res += getNodeValue(_n);
  if (res==0) res=1.0;
    result->setNodeValue(n, res);
  return res;
}
//=======================================================================
bool LeafMetric::run() {
  result->setAllNodeValue(0);
  result->setAllEdgeValue(0);
  node n;
  forEach(n, graph->getNodes())
    result->setNodeValue(n, getNodeValue(n));
  return true;
}
//=======================================================================
bool LeafMetric::check(std::string &erreurMsg) {
  if (AcyclicTest::isAcyclic(graph))
    return true;
  else {
    erreurMsg="The Graph must be a Acyclic";
    return false;
  }
}
//=======================================================================



