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

#ifndef _TLP_MIMES_H
#define _TLP_MIMES_H

#include <QtCore/QMimeData>

namespace tlp {
class Graph;
class WorkspacePanel;
class Algorithm;
class DataSet;
}

class GraphMimeType : public QMimeData {
public:
  void setGraph(tlp::Graph* graph) {
    _graph = graph;
  }
  
  tlp::Graph* graph() const {
    return _graph;
  }
  
private:
  tlp::Graph* _graph;
};

class AlgorithmMimeType : public QMimeData {
public:

  void setAlgorithm(tlp::Algorithm* algorithm) {
    _algorithm = algorithm;
  }
  
  tlp::Algorithm* algorithm() const {
    return _algorithm;
  }

  void setDataSet(tlp::DataSet* dataset) {
    _dataset = dataset;
  }
  
  tlp::DataSet* dataset() const {
    return _dataset;
  }
  
private:
  tlp::Algorithm* _algorithm;
  tlp::DataSet* _dataset;
};


class PanelMimeType : public QMimeData {
public:

  void setPanel(tlp::WorkspacePanel* panel) {
    _panel = panel;
  }
  
  tlp::WorkspacePanel* panel() const {
    return _panel;
  }
  
private:
  tlp::WorkspacePanel* _panel;
};

#endif //_TLP_MIMES_H