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
#ifndef Tulip_GLMAINVIEW_H
#define Tulip_GLMAINVIEW_H

#include <tulip/ViewWidget.h>
#include <tulip/CaptionItem.h>
#include <QtCore/QRectF>

class QGraphicsProxyWidget;

namespace tlp {
class GlOverviewGraphicsItem;
class SceneConfigWidget;
class GlMainWidget;
class QuickAccessBar;

class TLP_QT_SCOPE GlMainView: public tlp::ViewWidget {
  Q_OBJECT

  bool _overviewVisible;
  tlp::GlMainWidget* _glMainWidget;
  tlp::GlOverviewGraphicsItem* _overviewItem;
  tlp::SceneConfigWidget* _sceneConfigurationWidget;
  tlp::QuickAccessBar* _quickAccessBar;
  QGraphicsProxyWidget* _quickAccessBarItem;
  QPointF _captionPos;
  CaptionItem *_colorCaption;
  CaptionItem *_sizeCaption;

public:
  GlMainView();
  virtual ~GlMainView();
  tlp::GlMainWidget* getGlMainWidget() const;
  virtual QList<QWidget*> configurationWidgets() const;
  bool overviewVisible() const;
  void showHideCaption(CaptionItem::CaptionType captionType);
  QPixmap snapshot(const QSize &outputSize=QSize());

public slots:
  virtual void draw(tlp::PluginProgress* pluginProgress);
  virtual void refresh(PluginProgress *pluginProgress);
  virtual void drawOverview(bool generatePixmap=true);
  virtual void centerView();
  void setOverviewVisible(bool);
  void applySettings();

protected slots:
  virtual void glMainViewDrawn(GlMainWidget*,bool graphChanged);
  virtual void sceneRectChanged(const QRectF&);
  void setQuickAccessBarVisible(bool);
  void graphDeleted();

protected:
  virtual void setupWidget();
  bool quickAccessBarVisible() const;
};
}

#endif /* GLMAINVIEW_H_ */