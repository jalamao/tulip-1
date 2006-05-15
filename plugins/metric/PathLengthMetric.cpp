#include <tulip/AcyclicTest.h>
#include <tulip/ForEach.h>
#include "PathLengthMetric.h"

DOUBLEPLUGIN(PathLengthMetric,"Path Length","David Auber","15/02/2001","Alpha","0","2");

using namespace std;
using namespace tlp;

//=======================================
PathLengthMetric::PathLengthMetric(const PropertyContext &context):DoubleAlgorithm(context) {
}
//=======================================
double PathLengthMetric::getNodeValue(const node n) {
  if (graph->outdeg(n)==0) return 0.0;
  if (doubleResult->getNodeValue(n)!=0)
    return doubleResult->getNodeValue(n);
  double result=0;
  node child;
  forEach(child, graph->getOutNodes(n)) {
    result += doubleResult->getNodeValue(child);
  }
  result += leafMetric->getNodeValue(n);
  doubleResult->setNodeValue(n, result);
  return result;
}
//=======================================
bool PathLengthMetric::run() {
  doubleResult->setAllNodeValue(0);
  doubleResult->setAllEdgeValue(0);
  leafMetric = new DoubleProperty(graph);
  string erreurMsg;
  if (!graph->computeProperty("Leaf", leafMetric, erreurMsg)) {
    cerr << erreurMsg << endl;
    return false;
  }
  node _n;
  forEach(_n, graph->getNodes())
    getNodeValue(_n);
  delete leafMetric;
}
//=======================================
bool PathLengthMetric::check(string &erreurMsg) {
  if (AcyclicTest::isAcyclic(graph))
    return true;
  else {
    erreurMsg="The Graph must be acyclic";
    return false;
  }
}
//=======================================
