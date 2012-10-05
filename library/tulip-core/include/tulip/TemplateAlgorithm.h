/*
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

#ifndef TULIP_PROPERTY_H
#define TULIP_PROPERTY_H

#include <tulip/Algorithm.h>

namespace tlp {
class PluginContext;
static const std::string PROPERTY_ALGORITHM_CATEGORY = QObject::trUtf8("Property").toStdString();

/**
 * @ingroup Plugins
 * @brief A non-template interface for tlp::TemplateAlgorithm
 * @see tlp::TemplateAlgorithm
 **/
class TLP_SCOPE PropertyAlgorithm: public tlp::Algorithm {
public :
  PropertyAlgorithm(const tlp::PluginContext* context):Algorithm(context) {}
  virtual std::string category() const {
    return PROPERTY_ALGORITHM_CATEGORY;
  }
};

/**
 * @ingroup Plugins
 * @brief The TemplateAlgorithm class describes a plugin that can operate on a single graph's property.
 * @param Property The property template arguments gives the type of the property the algorithm operates on.
 *
 * A TemplateAlgorithm takes a graph as input (plus additional parameters defined via tlp::WithParameter) and outputs its results in a tlp::PropertyInterface subclass.
 * The output property is defined as an output parameter named "result" and as a class member called result.
 *
 * @warning Subclassing TemplateAlgorithm is not recommended since template specifications are available for every Tulip property types.
 *
 * @see tlp::BooleanAlgorithm
 * @see tlp::StringAlgorithm
 * @see tlp::DoubleAlgorithm
 * @see tlp::IntegerAlgorithm
 * @see tlp::LayoutAlgorithm
 * @see tlp::SizeAlgorithm
 */
template<class Property>
class TLP_SCOPE TemplateAlgorithm : public PropertyAlgorithm {
public:
  Property* result;
  TemplateAlgorithm (const tlp::PluginContext* context) : tlp::PropertyAlgorithm(context), result(NULL) {
    if (dataSet != NULL)
      dataSet->get("result", result);
  }
};


}
#endif
