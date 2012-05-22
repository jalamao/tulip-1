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
#ifndef GLOVERVIEWGRAPHICSITEM_H
#define GLOVERVIEWGRAPHICSITEM_H


#include <QtGui/QGraphicsPixmapItem>
#include <QtOpenGL/QGLFramebufferObject>
#include <tulip/GlScene.h>

namespace tlp {

class GlMainView;

class GlOverviewGraphicsItem : public QObject, public QGraphicsRectItem {

  Q_OBJECT

public:

  GlOverviewGraphicsItem(GlMainView *view,GlScene &scene);
  ~GlOverviewGraphicsItem();

  void setSize(unsigned int width, unsigned int height);

  void setLayerVisible(const std::string &name,bool visible);

public slots :

  void draw(bool generatePixmap);

private :

  void mousePressEvent(QGraphicsSceneMouseEvent* event);
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
  void setScenePosition(QPointF pos);


  GlMainView *view;
  GlScene &baseScene;
  unsigned int width, height;
  QGLFramebufferObject *glFrameBuffer;

  QGraphicsPixmapItem overview;
  QGraphicsLineItem line[4];
  QGraphicsPolygonItem poly[4];

  bool mouseClicked;

  std::set<std::string> _hiddenLayers;

};

}

#endif // GLOVERVIEWGRAPHICSITEM_H