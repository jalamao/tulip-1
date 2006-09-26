//-*-c++-*-

#include "tulip/StringAlgorithm.h"
#include "tulip/Graph.h"
#include "tulip/StringProperty.h"

tlp::StringAlgorithm::StringAlgorithm (const tlp::PropertyContext & context):tlp::PropertyAlgorithm<tlp::StringType , tlp::StringType>(context) {
  stringResult = (tlp::StringProperty *)context.propertyProxy;
}

