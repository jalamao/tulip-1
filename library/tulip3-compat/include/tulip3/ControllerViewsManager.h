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
#ifndef Tulip_CONTROLLERVIEWSMANAGER_H
#define Tulip_CONTROLLERVIEWSMANAGER_H

#include <tulip/tulipconf.h>

#include <map>
#include <string>

#include "tulip3/Controller.h"

class QWidget;
class QAction;

namespace tlp {

class Graph;
class View3;

/** \brief Views manager for Controller
 *
 * This class manage views for Controllers
 */
class TLP3_COMPAT_SCOPE ControllerViewsManager : public Controller {

  Q_OBJECT

public :

  ControllerViewsManager(tlp::PerspectiveContext &c);
  virtual ~ControllerViewsManager();

  /**
   * Attach the QMainWindow to the controller
   */
  void attachMainWindow(MainWindowFacade facade);

  /**
   * Return the Graph visualized by the controller
   * Equivalent to getCurrentGraph()
   */
  virtual Graph *getGraph();

  /**
   * Return the Graph visualized by the current view in the controller
   * Equivalent to getGraph()
   */
  virtual Graph *getCurrentGraph();

  /**
   * Set the graph visualized by the current view in the controller
   */
  virtual void setCurrentGraph(Graph *graph);

  /**
   * Return the current view in the controller
   */
  virtual View3 *getCurrentView();

  /**
   * Return the number of opened view
   */
  virtual unsigned int getViewsNumber()const;

  /**
   * Get the graph associated with the given view
   */
  virtual Graph *getGraphOfView(View3 *view);

  /**
   * return the views for the given graph.
   */
  virtual std::vector<View3*> getViewsOfGraph(Graph *graph);

  /**
   * Set the graph associated with the given view
   */
  virtual void setGraphOfView(View3 *view,Graph *graph);
  /**
    * Set the data of the given view.
    */
  virtual void setDataOfView(tlp::View3* view, tlp::Graph* graph, const tlp::DataSet& dataSet);
  /**
   * Get the view associated with the given widget
   */
  virtual View3 *getViewOfWidget(QWidget *widget);

  /**
   * Set the view associated with the given widget
   */
  virtual void setViewOfWidget(QWidget *widget,View3 *view);

  /**
   * Get views
   */
  virtual void getViews(std::vector<View3*> &views);

  /**
   * Get the widget associated with the given view
   */
  virtual QWidget *getWidgetOfView(View3 *view);

  /**
   * Get the name of the given view
   */
  virtual std::string getNameOfView(View3 *view) const;

  /**
   * Set the name of the given view
   */
  virtual void setNameOfView(View3 *view,const std::string &name);

  /**
   * Get the configuration widget of interactor activated for this view
   */
  virtual QWidget *getInteractorConfigurationWidgetOfView(View3 *view);

  /**
   * \brief Change the title of views of graph : graph
   * Through all opened views and change window title of view that visualize the graph
   * Window title is : view name + visualized graph name
   */
  virtual void changeWindowTitle(Graph *graph);

  /**
   * Close all the views.
   */
  virtual void closeAllViews();

  /**
   * Close the given view.
   */
  virtual void closeView(View3 *view);

  /**
   * Close all the views for the given graph.
   */
  virtual void closeViewsRelatedToGraph(Graph* graph);

  //*************************************************************************

  /**
   * Create a view with given name/graph/dataSet, set position and size of this view at rect and maximize it if maximized is at true
   * if forceWidgetSize is false : use rect size only if created widget have a size smaller than (10,10)
   * Return new created view
   */
  virtual View3* createView(const std::string &name,Graph *graph,DataSet dataSet,bool forceWidgetSize,const QRect &rect,bool maximized);

  /**
   * Initialize an already created view and set it into the workspace with the specified position, size, etc
   * See ControllerViewsManager::createView() for details.
   */
  void addView(View3 *view, Graph *graph, DataSet dataSet, bool forceWidgetSize, const QRect &rect, bool maximized, const std::string &viewName, QWidget *viewWidget);

  /**
   * Put interactors of the given view in graphToolBar
   */
  virtual void installInteractors(View3 *view);

  /**
   * Update view that use given graph (call setGraph on these views)
   */
  virtual void updateViewsOfGraph(Graph *graph);

  /**
   * Update view that use sub graph of given graph (call setGraph on these views)
   */
  virtual void updateViewsOfSubGraphs(Graph *graph);

  /**
   * Change graph of view that use oldGraph (call setGraph on these views)
   */
  virtual void changeGraphOfViews(Graph *oldGraph,Graph *newGraph);

  /**
   * Call draw/init on views, if call init() if init==true and draw() else
   */
  virtual void drawViews(bool init=false);

  /**
   * Store hierarchy of graphs for all views
   */
  virtual void saveViewsGraphsHierarchies();

  /**
   * Check hierarchy of graphs for all views
   */
  virtual void checkViewsGraphsHierarchy();

protected slots:

  /**
   * Create an empty view with action->text() name and currentGraph
   */
  virtual void createView(QAction *action);
  /**
   * This function is call when a view is activated
   * Return true if view can be activated (ie this view exist)
   */
  virtual bool windowActivated(QWidget *widget);
  /**
   * Load the interactor (in this function we catch the QAction who send this signal) in current View
   */
  virtual bool changeInteractor();
  /**
   * Load the interactor (referenced by the given QAction) in current View
   */
  virtual bool changeInteractor(QAction* action);
  /**
   * Load the interactor (referenced by the given QAction) in current View
   * ConfigurationWidget of this interactor is store in configurationWidget;
   */
  virtual bool changeInteractor(QAction* action,QWidget **configurationWidget);
  /**
   * Change the graph on the currentView
   */
  virtual bool changeGraph(Graph *graph);
  /**
   * This slot is call went a view will be closed
   */
  virtual void widgetWillBeClosed(QObject *object);

protected:
  View3 *currentView;
  std::map<View3 *, QAction *> lastInteractorOnView;

  Graph *currentGraph;

  std::map<View3 *,std::string> viewNames;
  std::map<QWidget *,View3*> viewWidget;
  std::map<View3 *,Graph* > viewGraph;

  std::map<View3 *, QWidget *> lastInteractorConfigurationWidgetOnView;
  std::map<View3 *,std::list<unsigned int> > viewsGraphsHierarchy;

};

}

#endif