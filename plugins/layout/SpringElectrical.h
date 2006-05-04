//-*-c++-*-
#ifndef _SPRINGELECTRICAL_H
#define _SPRINGELECTRICAL_H
#include <cmath>
#include <tulip/TulipPlugin.h>

class SpringElectrical:public LayoutAlgorithm { 
public:
  SpringElectrical(const PropertyContext &);
  bool run();

private:
  MutableContainer<double> sizeNorm;
  bool overlap(node u, Coord &move);
  bool checkEdgeIntersection(const node n, const Coord& move);
  Coord repulsiveForces(node n);
  Coord attractiveForces(node n);
  BooleanProperty* inputSelection;
  SizeProperty* inputSize;
  LayoutProperty* inputLayout;
  MutableContainer<Coord> prevMove;
  double k,k2; 
  Coord t;
};

#endif









