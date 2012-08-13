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

#ifndef QUICKACCESSBAR_H
#define QUICKACCESSBAR_H

#include <QtGui/QWidget>
#include <tulip/tulipconf.h>
#include <tulip/ScrollPopupButton.h>
#include <tulip/CaptionItem.h>

namespace Ui {
class QuickAccessBar;
}

namespace tlp {
class GlMainView;
class GlGraphRenderingParameters;
class GlGraphInputData;
class GlScene;

class QuickAccessBar : public QWidget {
  Q_OBJECT

  Ui::QuickAccessBar* _ui;
  QGraphicsItem *_quickAccessBarItem;
  tlp::GlMainView* _mainView;
  bool _resetting;
  tlp::GlGraphInputData* inputData() const;
  tlp::GlGraphRenderingParameters* renderingParameters() const;
  tlp::GlScene* scene() const;
  double _oldFontScale;
  double _oldNodeScale;
  bool _captionsInitialized;
  CaptionItem *_captions[4];
  void updateFontButtonStyle();
  void showHideCaption(CaptionItem::CaptionType captionType);

public:
  explicit QuickAccessBar(QGraphicsItem *quickAccessBarItem,QWidget *parent = 0);
  virtual ~QuickAccessBar();

public slots:
  void setGlMainView(tlp::GlMainView*);
  void reset();

  void setBackgroundColor(const QColor&);
  void setColorInterpolation(bool);
  void setLabelColor(const QColor&);
  void setNodeColor(const QColor&);
  void showHideNodesColorCaption();
  void showHideNodesSizeCaption();
  void showHideEdgesColorCaption();
  void showHideEdgesSizeCaption();
  void takeSnapshot();
  void setEdgesVisible(bool);
  void setLabelsVisible(bool);
  void selectFont();

signals:
  void settingsChanged();
};
}

#endif // QUICKACCESSBAR_H
