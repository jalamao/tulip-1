//-*-c++-*-

/*
 Author: David Auber
 Email : auber@labri.fr
 Last modification : 20/08/2001
 This program is free software; you can redistribute it and/or modify  *
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/

#ifndef TULIP_PROPERTY_H
#define TULIP_PROPERTY_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "PluginContext.h"
#include "Graph.h"
#include "Reflect.h"
#include "WithParameter.h"
#include "WithDependency.h"

namespace tlp {

/**
 * \addtogroup plugins
 */ 
/*@{*/
///
class TLP_SCOPE PropertyAlgorithm: public WithParameter, public WithDependency {
public :
  ///
  PropertyAlgorithm(const PropertyContext & context) :
    graph(context.graph),
    pluginProgress(context.pluginProgress),
    dataSet(context.dataSet) {
  }
  ///
  virtual ~PropertyAlgorithm(){}
  ///
  virtual bool run() {return true;}
  ///
  virtual bool check(std::string &) {return true;}

protected:
  Graph *graph;
  PluginProgress *pluginProgress;
  DataSet *dataSet;
};
/*@}*/

}
#endif
