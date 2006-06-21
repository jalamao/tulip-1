//-*-c++-*-
#ifndef _LEAFMETRIC_H
#define _LEAFMETRIC_H

#include <tulip/TulipPlugin.h>

/** \addtogroup metric */
/*@{*/
/// LeafMetric.h - Compute the number of leaves in the subtree induced by each node.
/** Computes the number of leaves in the subtree induced by each node.
 *
 *  <b>HISTORY</b>
 *
 *  - 20/08/2001 Verson 0.0.1: Initial release
 *
 *
 *  \note This algorithm assigns to each node a value defined as following : If two nodes are in the same
 *  connected component they have the same value else they have a 
 *  different value.
 *
 *
 *  \author David Auber University Bordeaux I France: Email:auber@tulip-software.org
 *
 *  <b>LICENCE</b>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by  
 *  the Free Software Foundation; either version 2 of the License, or     
 *  (at your option) any later version.
 *
 */
class LeafMetric:public DoubleAlgorithm { 
public:
  LeafMetric(const PropertyContext &);
  bool check(std::string &);
  bool run();
private:
  double getNodeValue(const node n);
};
/*@}*/
#endif



