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
#include <tulip/BaseGraphicsViewComponent.h>

#include <tulip/GWOverviewWidget.h>
#include <tulip/GlMainWidgetGraphicsView.h>
#include <tulip/ViewPluginsManager.h>
#include <tulip/GlMainView.h>
#include <tulip/TabWidgetHidableMenuGraphicsProxy.h>
#include <tulip/GlMainWidgetItem.h>

using namespace std;

namespace tlp {

BaseGraphicsViewComponent::BaseGraphicsViewComponent(const string &realViewName):realViewName(realViewName),graphicsView(NULL),baseView(ViewPluginsManager::getInst().createView(realViewName)) {}

BaseGraphicsViewComponent::~BaseGraphicsViewComponent() {
  delete baseView;
}

QWidget *BaseGraphicsViewComponent::construct(QWidget *parent) {
  QWidget *widget=AbstractView::construct(parent);
  QWidget* baseViewWidget = baseView->construct(parent);
  baseViewWidget->setObjectName("baseView Widget");
  baseViewWidget->hide();
  connect(baseView,SIGNAL(elementSelected(unsigned int,bool)),this,SLOT(elementSelectedSlot(unsigned int,bool)));

  widget->resize(512, 512);

  GWOverviewWidget *overviewWidget = static_cast<GlMainView *>(baseView)->getOverviewWidget();
  QAction *overviewAction = static_cast<GlMainView *>(baseView)->getOverviewAction();

  graphicsView = new GlMainWidgetGraphicsView(widget,static_cast<GlMainView *>(baseView)->getGlMainWidget());
  graphicsView->resize(512, 512);

  setCentralWidget(graphicsView);

  tabWidgetProxy = new TabWidgetHidableMenuGraphicsProxy(30);
  tabWidgetProxy->setPos(0,0);
  tabWidgetProxy->resize(370, 470);
  tabWidgetProxy->scale(0.7,0.7);
  tabWidgetProxy->hideTabWidget();
  tabWidgetProxy->setZValue(10);
  list<pair<QWidget *,string> > configWidgets=baseView->getConfigurationWidget();

  for(list<pair<QWidget *,string> >::iterator it=configWidgets.begin(); it!=configWidgets.end(); ++it) {
    tabWidgetProxy->addTab((*it).first, (*it).second.c_str());
  }

  graphicsView->scene()->addItem(tabWidgetProxy);

  overviewItem = NULL;

  if(overviewWidget) {
    overviewWidget->setDrawIfNotVisible(true);
    overviewItem=new GlMainWidgetItem(overviewWidget->getView(),100,100,true);
    overviewItem->setPos(0,0);
    overviewItem->setZValue(1);
    graphicsView->scene()->addItem(overviewItem);

    connect(overviewWidget, SIGNAL(hideOverview(bool)), this, SLOT(hideOverview(bool)));
    connect(overviewAction, SIGNAL(triggered(bool)), this, SLOT(setVisibleOverview(bool)));

    tabWidgetProxy->translate(0,overviewItem->boundingRect().height()+60);
  }

  return graphicsView;

}
void BaseGraphicsViewComponent::draw() {
  baseView->draw();
  graphicsView->draw();
}

void BaseGraphicsViewComponent::refresh() {
  baseView->refresh();
  graphicsView->redraw();
}

void BaseGraphicsViewComponent::init() {
  baseView->init();
  draw();
}

void BaseGraphicsViewComponent::elementSelectedSlot(unsigned int id,bool node) {
  emit elementSelected(id,node);
}

string BaseGraphicsViewComponent::getRealViewName() const {
  return realViewName;
}

void BaseGraphicsViewComponent::hideOverview(bool hide) {
  if(hide) {
    overviewItem->setVisible(false);
  }
  else {
    overviewItem->setVisible(true);
  }
}

void BaseGraphicsViewComponent::setVisibleOverview(bool visible) {
  hideOverview(!visible);
}


}

