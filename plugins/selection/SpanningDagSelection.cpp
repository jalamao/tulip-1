#include <cassert>

#include <tulip/Selection.h>
#include <tulip/AcyclicTest.h>

#include "SpanningDagSelection.h"

SELECTIONPLUGIN(SpanningDagSelection,"SpanningDag","David Auber","01/12/1999","Alpha","0","1");

using namespace std;

//=================================================================
SpanningDagSelection::SpanningDagSelection(const PropertyContext &context):SelectionAlgorithm(context) {}
//=================================================================
bool SpanningDagSelection::run() {
  selectionResult->setAllNodeValue(true);
  selectionResult->setAllEdgeValue(true);
  vector<edge> obstructions;
  AcyclicTest::acyclicTest(superGraph, &obstructions);
  for (vector<edge>::const_iterator it = obstructions.begin(); it != obstructions.end(); ++it) {
    selectionResult->setEdgeValue(*it, false);
  }
  return true;
}
//=================================================================
