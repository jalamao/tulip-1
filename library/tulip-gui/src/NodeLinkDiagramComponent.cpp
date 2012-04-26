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
#include "tulip/NodeLinkDiagramComponent.h"

#include "ui_GridOptionsWidget.h"

#include <QtGui/QGraphicsView>
#include <QtGui/QActionGroup>
#include <QtGui/QDialog>

#include <tulip/GlGrid.h>
#include <tulip/DrawingTools.h>
#include <tulip/TulipItemDelegate.h>
#include <tulip/ParameterListModel.h>
#include <tulip/GlMainWidget.h>
#include <tulip/GlGraphComposite.h>
#include <tulip/GlGraphInputData.h>
#include <tulip/Gl2DRect.h>
#include <tulip/ExtendedMetaNodeRenderer.h>
#include <tulip/GlVertexArrayManager.h>

using namespace tlp;
using namespace std;

NodeLinkDiagramComponent::NodeLinkDiagramComponent(const tlp::PluginContext*): _grid(NULL), _gridOptions(NULL),_hasHulls(false) {
}

void NodeLinkDiagramComponent::updateGrid() {
  delete _grid;
  _grid = NULL;

  if(_gridOptions==NULL)
    return;

  DataSet gridData = static_cast<ParameterListModel*>(_gridOptions->findChild<QTableView*>()->model())->parametersValues();
  StringCollection gridMode;
  gridData.get<StringCollection>("Grid mode",gridMode);
  int mode = gridMode.getCurrent();

  if (mode == 0)
    return;

  Coord margins;
  Size gridSize;
  Color gridColor;
  bool onX=true,onY=true,onZ=true;
  gridData.get<Coord>("Margins",margins);
  gridData.get<Size>("Grid size",gridSize);
  gridData.get<Color>("Grid color",gridColor);
  gridData.get<bool>("X grid",onX);
  gridData.get<bool>("Y grid",onY);
  gridData.get<bool>("Z grid",onZ);

  GlGraphInputData* inputData = getGlMainWidget()->getScene()->getGlGraphComposite()->getInputData();
  BoundingBox graphBB = computeBoundingBox(graph(),inputData->getElementLayout(),inputData->getElementSize(),inputData->getElementRotation());
  Coord bottomLeft = Coord(graphBB[0] - margins);
  Coord topRight = Coord(graphBB[1] + margins);

  if (mode == 1) {
    for (int i=0; i<3; ++i)
      gridSize[i] = abs(topRight[i] - bottomLeft[i]) / gridSize[i];
  }

  bool displays[3];
  displays[0] = onX;
  displays[1] = onY;
  displays[2] = onZ;

  _grid = new GlGrid(bottomLeft,
                     topRight,
                     gridSize,
                     gridColor,
                     displays);
  getGlMainWidget()->getScene()->getLayer("Main")->addGlEntity(_grid,"Node Link Diagram Component grid");
}

void NodeLinkDiagramComponent::draw(PluginProgress *pluginProgress) {
  updateGrid();
  GlMainView::draw(pluginProgress);
}

void NodeLinkDiagramComponent::setState(const tlp::DataSet& data) {
  ParameterDescriptionList gridParameters;
  gridParameters.add<StringCollection>("Grid mode","","No grid;Space divisions;Fixed size",true);
  gridParameters.add<Size>("Grid size","","(1,1,1)",false);
  gridParameters.add<Size>("Margins","","(0.5,0.5,0.5)",false);
  gridParameters.add<Color>("Grid color","","(0,0,0,255)",false);
  gridParameters.add<bool>("X grid","","true",false);
  gridParameters.add<bool>("Y grid","","true",false);
  gridParameters.add<bool>("Z grid","","true",false);
  ParameterListModel* model = new ParameterListModel(gridParameters,NULL,this);

  Ui::GridOptionsWidget* ui = new Ui::GridOptionsWidget;
  _gridOptions = new QDialog(graphicsView());
  ui->setupUi(_gridOptions);
  ui->tableView->setModel(model);
  ui->tableView->setItemDelegate(new TulipItemDelegate);
  connect(ui->tableView, SIGNAL(destroyed()), ui->tableView->itemDelegate(), SLOT(deleteLater()));

  bool overviewVisible=true;

  if(data.exist("overviewVisible")) {
    data.get<bool>("overviewVisible",overviewVisible);
  }

  bool quickAccessBarVisible;

  if(data.exist("quickAccessBarVisible")) {
    data.get<bool>("quickAccessBarVisible",quickAccessBarVisible);
  }

  createScene(graph(), data);
  registerTriggers();

  graphicsView()->setContextMenuPolicy(Qt::ActionsContextMenu);

  QAction* overviewAction = new QAction(trUtf8("Overview"),this);
  overviewAction->setCheckable(true);
  overviewAction->setChecked(overviewVisible);
  connect(overviewAction,SIGNAL(triggered(bool)),this,SLOT(setOverviewVisible(bool)));
  graphicsView()->addAction(overviewAction);

  QAction* viewSeparator = new QAction(trUtf8("View"),this);
  viewSeparator->setSeparator(true);
  graphicsView()->addAction(viewSeparator);

  QAction* redrawAction = new QAction(trUtf8("Redraw"),this);
  redrawAction->setShortcut(trUtf8("Ctrl+Shift+R"));
  connect(redrawAction,SIGNAL(triggered()),this,SLOT(redraw()));
  graphicsView()->addAction(redrawAction);

  QAction* centerAction = new QAction(trUtf8("Center"),this);
  centerAction->setShortcut(trUtf8("Ctrl+Shift+C"));
  connect(centerAction,SIGNAL(triggered()),getGlMainWidget(),SLOT(centerScene()));
  graphicsView()->addAction(centerAction);

  QAction* augmentedSeparator = new QAction(trUtf8("Augmented display"),this);
  augmentedSeparator->setSeparator(true);
  graphicsView()->addAction(augmentedSeparator);

  QAction* zOrderAction = new QAction(trUtf8("Z Ordering"),this);
  zOrderAction->setCheckable(true);
  zOrderAction->setChecked(false);
  connect(redrawAction,SIGNAL(triggered(bool)),this,SLOT(setZOrdering(bool)));
  graphicsView()->addAction(zOrderAction);

  QAction* gridAction = new QAction(trUtf8("Grid"),this);
  connect(gridAction,SIGNAL(triggered()),this,SLOT(showGridControl()));
  graphicsView()->addAction(gridAction);


  setOverviewVisible(overviewVisible);

  if(overviewVisible)
    drawOverview(true);

  setQuickAccessBarVisible(true);
}

void NodeLinkDiagramComponent::graphChanged(tlp::Graph* graph) {
  loadGraphOnScene(graph);
  registerTriggers();
  emit drawNeeded();
}

tlp::DataSet NodeLinkDiagramComponent::state() const {
  DataSet data=sceneData();
  data.set("overviewVisible",overviewVisible());
  data.set("quickAccessBarVisible",quickAccessBarVisible());
  return data;
}

//==================================================
void NodeLinkDiagramComponent::createScene(Graph *graph,DataSet dataSet) {
  GlScene *scene=getGlMainWidget()->getScene();
  scene->clearLayersList();

  std::string sceneInput="";

  if(dataSet.exist("scene")) {
    dataSet.get("scene",sceneInput);
  }

  if(sceneInput=="") {
    //Default scene
    GlLayer* layer=new GlLayer("Main");
    GlLayer *backgroundLayer=new GlLayer("Background");
    backgroundLayer->setVisible(false);
    GlLayer *foregroundLayer=new GlLayer("Foreground");
    foregroundLayer->setVisible(true);

    backgroundLayer->set2DMode();
    foregroundLayer->set2DMode();
    std::string dir=TulipBitmapDir;
    Gl2DRect *background=new Gl2DRect(0,1.,0,1.,dir + "tex_back.png",true);
    backgroundLayer->addGlEntity(background,"background");

    Gl2DRect *labri=new Gl2DRect(5.,5.,50.,50.,dir + "logolabri.jpg",true,false);
    labri->setVisible(false);
    foregroundLayer->addGlEntity(labri,"labrilogo");

    scene->addExistingLayer(backgroundLayer);
    scene->addExistingLayer(layer);
    scene->addExistingLayer(foregroundLayer);
    GlGraphComposite* graphComposite=new GlGraphComposite(graph);
    scene->getLayer("Main")->addGlEntity(graphComposite,"graph");
    graphComposite->getRenderingParametersPointer()->setViewNodeLabel(true);
    graphComposite->getRenderingParametersPointer()->setEdgeColorInterpolate(false);
    scene->centerScene();
  }
  else {
    size_t pos=sceneInput.find("TulipBitmapDir/");

    while(pos!=std::string::npos) {
      sceneInput.replace(pos,15,TulipBitmapDir);
      pos=sceneInput.find("TulipBitmapDir/");
    }

    pos=sceneInput.find("TulipLibDir/");

    while(pos!=std::string::npos) {
      sceneInput.replace(pos,12,TulipLibDir);
      pos=sceneInput.find("TulipLibDir/");
    }

    scene->setWithXML(sceneInput,graph);
  }

  if(dataSet.exist("Display")) {
    DataSet renderingParameters;
    dataSet.get("Display",renderingParameters);
    GlGraphRenderingParameters rp=scene->getGlGraphComposite()->getRenderingParameters();
    rp.setParameters(renderingParameters);
    scene->getGlGraphComposite()->setRenderingParameters(rp);
  }

  if(dataSet.exist("Hulls")) {
    useHulls(true);
    DataSet hullsSet;
    dataSet.get<DataSet>("Hulls", hullsSet);
    manager->setVisible(true);
    manager->setData(hullsSet);
  }

  scene->getGlGraphComposite()->getInputData()->setMetaNodeRenderer(new ExtendedMetaNodeRenderer(scene->getGlGraphComposite()->getInputData()));
  getGlMainWidget()->emitGraphChanged();
}
//==================================================
DataSet NodeLinkDiagramComponent::sceneData() const {
  GlScene *scene=getGlMainWidget()->getScene();
  DataSet outDataSet;
  outDataSet.set<DataSet>("Display",scene->getGlGraphComposite()->getRenderingParameters().getParameters());
  std::string out;
  scene->getXML(out);
  size_t pos=out.find(TulipBitmapDir);

  while(pos!=std::string::npos) {
    out.replace(pos,TulipBitmapDir.size(),"TulipBitmapDir/");
    pos=out.find(TulipBitmapDir);
  }

  outDataSet.set<std::string>("scene",out);

  if(_hasHulls && manager->isVisible()) {
    outDataSet.set<DataSet>("Hulls", manager->getData());
  }

  return outDataSet;
}
//==================================================
void NodeLinkDiagramComponent::loadGraphOnScene(Graph *graph) {
  GlScene *scene=getGlMainWidget()->getScene();

  if(!scene->getLayer("Main")) {
    createScene(graph,DataSet());
    return;
  }

  if(_hasHulls)
    manager->setGraph(graph);

  GlGraphComposite* oldGraphComposite=(GlGraphComposite *)(scene->getLayer("Main")->findGlEntity("graph"));

  if(!oldGraphComposite) {
    createScene(graph,DataSet());
    return;
  }

  GlGraphRenderingParameters param=oldGraphComposite->getRenderingParameters();
  GlMetaNodeRenderer *metaNodeRenderer=oldGraphComposite->getInputData()->getMetaNodeRenderer();
  oldGraphComposite->getInputData()->setMetaNodeRenderer(NULL,false);
  GlGraphComposite* graphComposite=new GlGraphComposite(graph);
  graphComposite->setRenderingParameters(param);

  metaNodeRenderer->setInputData(graphComposite->getInputData());

  graphComposite->getInputData()->setMetaNodeRenderer(metaNodeRenderer);

  if(oldGraphComposite->getInputData()->graph==graph) {
    oldGraphComposite->getInputData()->deleteGlVertexArrayManagerInDestructor(false);
    delete graphComposite->getInputData()->getGlVertexArrayManager();
    graphComposite->getInputData()->setGlVertexArrayManager(oldGraphComposite->getInputData()->getGlVertexArrayManager());
    graphComposite->getInputData()->getGlVertexArrayManager()->setInputData(graphComposite->getInputData());
  }

  scene->getLayer("Main")->addGlEntity(graphComposite,"graph");

  delete oldGraphComposite;
  getGlMainWidget()->emitGraphChanged();
}

void NodeLinkDiagramComponent::registerTriggers() {
  clearRedrawTriggers();

  if (graph() == NULL)
    return;

  addRedrawTrigger(getGlMainWidget()->getScene()->getGlGraphComposite()->getGraph());
  std::set<tlp::PropertyInterface*> properties = getGlMainWidget()->getScene()->getGlGraphComposite()->getInputData()->properties();

  for(std::set<tlp::PropertyInterface*>::iterator it = properties.begin(); it != properties.end(); ++it) {
    addRedrawTrigger(*it);
  }
}

void NodeLinkDiagramComponent::redraw() {
  getGlMainWidget()->redraw();
}

void NodeLinkDiagramComponent::setZOrdering(bool f) {
  getGlMainWidget()->getScene()->getGlGraphComposite()->getRenderingParametersPointer()->setElementZOrdered(f);
}

void NodeLinkDiagramComponent::showGridControl() {
  if (_gridOptions->exec() == QDialog::Rejected)
    return;

  updateGrid();
  emit drawNeeded();
}

void NodeLinkDiagramComponent::useHulls(bool hasHulls) {
  GlScene *scene=getGlMainWidget()->getScene();

  if(hasHulls == _hasHulls)
    return;

  _hasHulls = hasHulls;

  if(_hasHulls) {
    manager = new GlCompositeHierarchyManager(scene->getGlGraphComposite()->getInputData()->getGraph(),
        scene->getLayer("Main"),
        "Hulls",
        scene->getGlGraphComposite()->getInputData()->getElementLayout(),
        scene->getGlGraphComposite()->getInputData()->getElementSize(),
        scene->getGlGraphComposite()->getInputData()->getElementRotation());
    // Now we remove and add GlGraphComposite to be sure of the order (first Hulls and after GraphComposite)
    // This code doesn't affect the behavior of tulip but the tlp file is modified
    scene->getLayer("Main")->deleteGlEntity(scene->getGlGraphComposite());
    scene->getLayer("Main")->addGlEntity(scene->getGlGraphComposite(),"graph");
  }
}

bool NodeLinkDiagramComponent::hasHulls() const {
  return _hasHulls;
}

PLUGIN(NodeLinkDiagramComponent)