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
#include "tulip3/Controller.h"

#include <tulip3/ControllerAlgorithmTools.h>
#include <QtGui/QWorkspace>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <tulip/SizeProperty.h>
#include <tulip/ColorProperty.h>
#include <tulip/IntegerProperty.h>
#include <tulip/StringProperty.h>
#include <tulip/EdgeExtremityGlyphManager.h>
#include <tulip/TulipProject.h>
#include <tulip/PluginManager.h>
#include <tulip/TlpQtTools.h>
#include <tulip/SimplePluginProgressWidget.h>
#include "tulip3/QtProgress.h"
#include <QtCore/QDebug>
#include <tulip3/Tlp3Tools.h>

#include "ui_Tulip.h"

using namespace std;

//====================================================
tlp::Controller* tlp::Controller::currentController=0;

namespace tlp {
MainWindowFacade::MainWindowFacade()
  :mainWindow(NULL),menuBar(NULL),toolBar(NULL),interactorsToolBar(NULL),workspace(NULL),statusBar(NULL) {
}
MainWindowFacade::MainWindowFacade(QMainWindow *mainWindow,QToolBar *toolBar,QToolBar *interactorsToolBar,QWorkspace *workspace)
  :mainWindow(mainWindow),menuBar(mainWindow->menuBar()),toolBar(toolBar),interactorsToolBar(interactorsToolBar),workspace(workspace),statusBar(mainWindow->statusBar()) {
}

Controller::Controller(tlp::PerspectiveContext &c): Perspective(c) {
}

void Controller::start(tlp::PluginProgress *) {
  _buildUi();

  tlp::Graph *g;
  DataSet controllerData;

  if (_externalFile.isEmpty()) {
    g = tlp::newGraph();
    initializeGraph(g);

    if (_parameters.contains("import"))
      runImportPlugin(_parameters["import"].toString(),g);
  }
  else {
    QtProgress progress(_mainWindow, "Loading file");
    progress.show();
    DataSet importData;
    importData.set("file::filename",_externalFile.toStdString());
    _lastUsedSavePath = _externalFile;
    g = tlp::importGraph("tlpimport",importData,&progress);

    if (!g) {
      g = tlp::newGraph();
      initializeGraph(g);
    }

    if(importData.exist("controller")) {
      DataSet tmp;
      importData.get<DataSet>("controller", tmp);
      Iterator< std::pair<std::string, DataType*> > *it=tmp.getValues();
      std::pair<std::string, DataType*> p;
      p = it->next();
      controllerData=*((DataSet*)p.second->value);
    }
  }

  setData(g,controllerData);
}

void Controller::attachMainWindow(MainWindowFacade facade) {
  mainWindowFacade=facade;
}

void Controller::_buildUi() {
  Ui::TulipData *ui = new Ui::TulipData;
  ui->setupUi(_mainWindow);
  _mainWindow->show();

  QGridLayout *gridLayout = new QGridLayout(ui->tab);
  QWorkspace *workspace = new QWorkspace(ui->tab);
  workspace->setBackground(QBrush(QPixmap(QString::fromUtf8(":/background_logo.png"))));
  gridLayout->addWidget(workspace, 0, 0, 1, 1);

  importMenu = new QMenu(trUtf8("Import"));
  updateImportExportMenus();
  ui->fileMenu->insertMenu(ui->filePrintAction,importMenu);

  connect(ui->fileOpenAction,SIGNAL(triggered()),this,SIGNAL(showOpenProjectWindow()));
  connect(ui->fileSaveAction,SIGNAL(triggered()),this,SLOT(save()));
  connect(ui->fileSaveAsAction,SIGNAL(triggered()),this,SLOT(saveAs()));
  connect(ui->helpDocumentationAction,SIGNAL(triggered()),this,SIGNAL(showTulipAboutPage()));
  connect(ui->helpDocumentationAction,SIGNAL(triggered()),this,SIGNAL(showTulipAboutPage()));
  connect(ui->pluginsAction,SIGNAL(triggered()),this,SIGNAL(showTulipPluginsCenter()));
  connect(ui->helpAboutAction,SIGNAL(triggered()),this,SIGNAL(showTulipAboutPage()));
  connect(ui->windowsMenu, SIGNAL( aboutToShow() ), this, SLOT( windowsMenuAboutToShow()));
  windowsMenu = ui->windowsMenu;
  attachMainWindow(MainWindowFacade(_mainWindow,ui->toolBar,ui->graphToolBar,workspace));

}

void Controller::updateImportExportMenus() {
  QList<tlp::PluginInformations *> localPlugins = PluginManager::pluginsList(PluginManager::Local);
  tlp::PluginInformations *infos;
  QMap<QString,QMenu *> groupMenus;
  foreach(infos,localPlugins) {
    if (infos->type() != "ImportModule")
      continue;

    QMenu *parent = importMenu;

    if (groupMenus.contains(infos->group()))
      parent = groupMenus[infos->group()];
    else if (!infos->group().isEmpty()) {
      parent = importMenu->addMenu(infos->group());
      groupMenus[infos->group()] = parent;
    }

    QAction *action = parent->addAction(infos->name());;
    connect(action,SIGNAL(triggered()),this,SLOT(importMenuClicked()));
  }
}

void Controller::importMenuClicked() {
  QAction *src = static_cast<QAction *>(sender());

  if (isEmpty())
    runImportPlugin(src->text(),getGraph());
  else {
    QVariantMap params;
    params["import"]=src->text();
    emit createPerspective(_project->perspective(),params);
  }
}

void Controller::runImportPlugin(const QString &name, Graph *g) {
  DataSet dataSet;
  ParameterList *params = ControllerAlgorithmTools::getPluginParameters(PluginLister<ImportModule,AlgorithmContext>::getInstance(),name.toStdString());
  ParameterList sysDef = ImportModuleLister::getPluginParameters(name.toStdString());
  params->buildDefaultDataSet(dataSet,g);
  string title = string("Tulip Parameter Editor: ") + name.toStdString();
  bool ok = tlp3::openDataSetDialog(dataSet, &sysDef, params, &dataSet, title.c_str(), g, _mainWindow);

  if (ok) {
    Observable::holdObservers();
    SimplePluginProgressDialog *progress = new SimplePluginProgressDialog(_mainWindow);
    progress->show();
    tlp::importGraph(name.toStdString(),dataSet,progress->progress(),g);
    Observable::unholdObservers();
    delete progress;
  }
}

void Controller::save() {
  if (_lastUsedSavePath.isEmpty())
    saveAs();
  else
    saveAs(_lastUsedSavePath);
}

void Controller::saveAs() {
  saveAs(QFileDialog::getSaveFileName(0,trUtf8("Save File"), _lastUsedSavePath, trUtf8("Tulip graphs (*.tlp)")));
}

void Controller::saveAs(const QString &file) {
  if (file.isEmpty())
    return;

  ofstream os(file.toStdString().c_str());

  Graph *g;
  DataSet dataSet;
  getData(&g,&dataSet);
  tlp::SimplePluginProgressDialog *progress = new tlp::SimplePluginProgressDialog(0);
  tlp::exportGraph(g,os,"tlpexport",dataSet,progress->progress());
  delete progress;
  _lastUsedSavePath = file;
}

void Controller::initializeGraph(Graph *graph) {
  graph->getProperty<SizeProperty>("viewSize")->setAllNodeValue(Size(1,1,1));
  graph->getProperty<SizeProperty>("viewSize")->setAllEdgeValue(Size(0.125,0.125,0.5));
  graph->getProperty<ColorProperty>("viewColor")->setAllNodeValue(Color(255,0,0));
  graph->getProperty<ColorProperty>("viewColor")->setAllEdgeValue(Color(0,0,0));
  graph->getProperty<IntegerProperty>("viewShape")->setAllNodeValue(4);
  graph->getProperty<IntegerProperty>("viewShape")->setAllEdgeValue(0);
  graph->getProperty<StringProperty>("viewFont")->setAllNodeValue(tlp::TulipBitmapDir + "font.ttf");
  graph->getProperty<StringProperty>("viewFont")->setAllEdgeValue(tlp::TulipBitmapDir + "font.ttf");
  graph->getProperty<IntegerProperty>("viewFontSize")->setAllNodeValue(18);
  graph->getProperty<IntegerProperty>("viewFontSize")->setAllEdgeValue(18);
  graph->getProperty<IntegerProperty>("viewSrcAnchorShape")->setAllEdgeValue(EdgeExtremityGlyphManager::NoEdgeExtremetiesId);
  graph->getProperty<SizeProperty>("viewSrcAnchorSize")->setAllEdgeValue(Size(0.25,0.25,0.25));
  graph->getProperty<IntegerProperty>("viewTgtAnchorShape")->setAllEdgeValue(50);
  graph->getProperty<SizeProperty>("viewTgtAnchorSize")->setAllEdgeValue(Size(0.25,0.25,0.25));
}

bool Controller::terminated() {
  if (!isEmpty() && QMessageBox::question(0, trUtf8("Exiting"),trUtf8("Do you want to save your graph?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
    save();

  return true;
}

bool Controller::isEmpty() {
  return getGraph()->numberOfNodes() == 0;
}

//**********************************************************************
void Controller::windowsMenuAboutToShow() {
  windowsMenu->clear();
  QAction* cascadeAction = windowsMenu->addAction("&Cascade", this, SLOT(cascade() ) );
  QAction* tileAction = windowsMenu->addAction("&Tile", mainWindowFacade.getWorkspace(), SLOT(tile() ) );
  QAction* closeallAction = windowsMenu->addAction("Close All", this, SLOT(closeAll()));

  if ( mainWindowFacade.getWorkspace()->windowList().isEmpty() ) {
    cascadeAction->setEnabled(false);
    tileAction->setEnabled(false);
    closeallAction->setEnabled(false);
  }
  else {
    windowsMenu->addSeparator();
    QWidgetList windows = mainWindowFacade.getWorkspace()->windowList();

    for ( int i = 0; i < int(windows.count()); ++i ) {
      QAction* action = windowsMenu->addAction(windows.at(i)->windowTitle());
      action->setChecked(mainWindowFacade.getWorkspace()->activeWindow() == windows.at(i));
      action->setData(QVariant(i));
    }
  }
}
void Controller::cascade() {
  mainWindowFacade.getWorkspace()->cascade();
  QWidgetList widgetList=mainWindowFacade.getWorkspace()->windowList();

  for(QWidgetList::iterator it=widgetList.begin(); it!=widgetList.end(); ++it) {
    (*it)->resize(QSize(512,512));
  }
}

void Controller::closeAll() {
  mainWindowFacade.getWorkspace()->closeAllWindows();
}

}