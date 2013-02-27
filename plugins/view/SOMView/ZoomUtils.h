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

#ifndef ZOOMUTILS_H_
#define ZOOMUTILS_H_

#include <tulip/BoundingBox.h>

namespace tlp {
class GlMainWidget;
}
void zoomOnScreenRegion(tlp::GlMainWidget *glWidget, const tlp::BoundingBox &boundingBox, const bool optimalPath = true, const double velocity = 1.1, const double p = sqrt(1.6));
void zoomOnScreenRegionWithoutAnimation(tlp::GlMainWidget *glWidget, const tlp::BoundingBox &boundingBox);

#endif /* ZOOMUTILS_H_ */
