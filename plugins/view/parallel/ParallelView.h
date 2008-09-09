//-*-c++-*-
/*
 Author: Morgan MATHIAUT
 Email : mathiaut@labri.fr
 Last modification : 06/08

 This program is free software; you can redistribute it and/or modify  *
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/

#ifndef Tulip_PARALLELVIEW_H
#define Tulip_PARALLELVIEW_H

#include <QtGui/QMainWindow>

#include <tulip/View.h>

#include "ParallelCoordinatesWidget.h"
#include "ParallelViewData.h"

namespace tlp {

  class ParallelCoordinatesViewCreator : public ViewCreator {
  public :
    virtual View* create(const std::string &pluginName,QWidget *parent);
  };

  class ParallelView : public View, public Ui::ParallelViewData {

    Q_OBJECT;

    ParallelCoordinatesWidget *mainWidget;

  public :
    
    ParallelView(const std::string &pluginName,QWidget *parent = 0);
    ~ParallelView();
    
    void setData(Graph *graph,DataSet dataSet);
    DataSet getData();
    Graph *getGraph();
    QWidget* getWidget() {return this;}

    void getInteractorsActionList(std::list<QAction*> &interactorsList);
    Iterator<Interactor *> *installInteractor(const std::string &);
    void constructInteractorsMap();

  public slots:
    void showDialog(QAction*);
    
  };

}

#endif // Tulip_PARALLELVIEW_H
