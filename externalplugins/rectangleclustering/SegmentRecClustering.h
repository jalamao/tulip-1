//-*-c++-*-
#ifndef _SEGMENTRECCLUSTERING_H
#define _SEGMENTRECCLUSTERING_H

#include <tulip/TulipPlugin.h>
/// SegmentRecClustering.h - A tree clustering algorithm.
/** This plugin implement the statistical tree clustering algorithm 
 *  first published as :
 *
 *  D. Auber and M. Delest,
 *  "A clustering algorithm for huge trees",
 *  "Advances in Applied Mathematics",
 *  "To appear (Accepted 2002)",
 *  pages "1--14"
 *  publisher "Academic press"
 *
 *  <b> HISTORY </b>
 *
 *  - 01/01/2002 Verson 0.0.1: Initial release
 *
 *  
 *  \note This version is the one which use only maximum segment length to make clustering.
 *
 *  
 *  \author David Auber University Bordeaux I France, Email : auber@tulip-software.com
 *
 *  <b> LICENCE </b>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by  
 *  the Free Software Foundation; either version 2 of the License, or     
 *  (at your option) any later version.
 *
 */
class SegmentRecClustering:public Clustering { 
public:
  SegmentRecClustering(ClusterContext);
  ~SegmentRecClustering();
  bool run();
  bool check(std::string &);
  void reset();

private:
  bool DfsClustering (node ,BooleanProperty *,BooleanProperty *);
  void getRecurseChild(node,BooleanProperty *,BooleanProperty *);
  DoubleProperty *segmentM;
  DoubleProperty *leafM;
  DoubleProperty *nodeM;
  int mResult;
  double inter;
};

#endif
