/**
 *
 * This file is part of Tulip (www.tulip-software.org)
 *
 * Authors: David Auber and the Tulip development Team
 * from LaBRI, University of Bordeaux
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
#include "tulip/GraphHierarchiesModel.h"

#include <QFont>
#include <QSize>
#include <QDebug>
#include <QMimeData>

#include <tulip/TlpTools.h>
#include <tulip/TulipMetaTypes.h>
#include <tulip/Graph.h>
#include <tulip/TulipProject.h>
#include <tulip/IntegerProperty.h>
#include <tulip/ColorProperty.h>
#include <tulip/SizeProperty.h>
#include <tulip/DoubleProperty.h>
#include <tulip/TulipSettings.h>
#include <tulip/TulipMimes.h>
#include <tulip/DrawingTools.h>
#include <tulip/EdgeExtremityGlyphManager.h>
#include <tulip/GraphNeedsSavingObserver.h>
#include <tulip/TlpQtTools.h>

#include <fstream>

using namespace std;
using namespace tlp;

#define NAME_SECTION 0
#define ID_SECTION 1
#define NODES_SECTION 2
#define EDGES_SECTION 3

// define some specific MetaValueCalculator classes
// viewColor
class ViewColorCalculator :public AbstractColorProperty::MetaValueCalculator {
public:
  virtual void computeMetaValue(AbstractColorProperty* color, node mN,
                                Graph*, Graph*) {
    // meta node color is half opaque white
    color->setNodeValue(mN, Color(255, 255, 255, 127));
  }

  virtual void computeMetaValue(AbstractColorProperty* color, edge mE,
                                Iterator<edge>*itE, Graph*) {
    // meta edge color is the color of the first underlying edge
    color->setEdgeValue(mE, color->getEdgeValue(itE->next()));
  }
};

// viewBorderWidth
class ViewBorderWidthCalculator :public DoubleMinMaxProperty::MetaValueCalculator {
public:
  virtual void computeMetaValue(AbstractProperty<DoubleType, DoubleType, NumericProperty>* width, node mN,
                                Graph*, Graph*) {
    // meta node border width is 1
    width->setNodeValue(mN, 1);
  }
};

// viewLabel
class ViewLabelCalculator :public AbstractStringProperty::MetaValueCalculator {
public:
  // set the meta node label to label of viewMetric max corresponding node
  void computeMetaValue(AbstractStringProperty* label,
                        node mN, Graph* sg, Graph*) {
    // nothing to do if viewMetric does not exist
    if (!sg->existProperty("viewMetric"))
      return;

    node viewMetricMaxNode;
    double vMax = -DBL_MAX;
    DoubleProperty *metric = sg->getProperty<DoubleProperty>("viewMetric");
    Iterator<node> *itN= sg->getNodes();

    while (itN->hasNext()) {
      node itn = itN->next();
      const double& value = metric->getNodeValue(itn);

      if (value > vMax) {
        vMax = value;
        viewMetricMaxNode = itn;
      }
    }

    delete itN;

    if (viewMetricMaxNode.isValid())
      label->setNodeValue(mN, label->getNodeValue(viewMetricMaxNode));
  }
};

// corresponding static instances
static ViewColorCalculator vColorCalc;
static ViewLabelCalculator vLabelCalc;
static ViewBorderWidthCalculator vWidthCalc;

GraphHierarchiesModel::GraphHierarchiesModel(QObject *parent): TulipModel(parent), _currentGraph(NULL) {}

GraphHierarchiesModel::GraphHierarchiesModel(const GraphHierarchiesModel &copy): TulipModel(copy.QObject::parent()), tlp::Observable() {
  for (int i=0; i < copy.size(); ++i)
    addGraph(copy[i]);

  _currentGraph = NULL;
}

GraphHierarchiesModel::~GraphHierarchiesModel() {
  qDeleteAll(_saveNeeded);
}

// Cache related methods
QModelIndex GraphHierarchiesModel::indexOf(const tlp::Graph* g) {
  if (g == NULL)
    return QModelIndex();

  QModelIndex result = _indexCache[g];

  if (!result.isValid())
    result = forceGraphIndex(const_cast<Graph*>(g));

  return result;
}

QModelIndex GraphHierarchiesModel::forceGraphIndex(Graph* g) {
  if (g == NULL)
    return QModelIndex();

  QModelIndex result;

  if (g->getRoot() == g) { // Peculiar case for root graphs
    result = createIndex(_graphs.indexOf(g),0,g);
    _indexCache[g] = result;
  }
  else {
    Graph* parent = g->getSuperGraph();
    unsigned int n = 0;

    for (n=0; n<parent->numberOfSubGraphs(); ++n) {
      if (parent->getNthSubGraph(n) == g)
        break;
    }

    result = createIndex(n,0,g);
    _indexCache[g] = result;
  }

  return result;
}

// Serialization
static QString GRAPHS_PATH("/graphs/");
//static QString GRAPH_FILE("graph.json");

bool GraphHierarchiesModel::needsSaving() {
  bool saveNeeded = false;
  foreach(GraphNeedsSavingObserver* observer, _saveNeeded) {
    saveNeeded = saveNeeded || observer->needsSaving();
  }
  return saveNeeded;
}

QMap<QString,tlp::Graph*> GraphHierarchiesModel::readProject(tlp::TulipProject *project, tlp::PluginProgress *progress) {
  QMap<QString,tlp::Graph*> rootIds;
  foreach(QString entry, project->entryList(GRAPHS_PATH,QDir::Dirs | QDir::NoDot | QDir::NoDotDot, QDir::Name)) {
    QString file = GRAPHS_PATH + entry + "/graph.tlp";

//    if (!project->exists(file)) {
//      file = GRAPHS_PATH + entry + "/graph.json";

    if (!project->exists(file))
      continue;

//    }

    QString absolutePath = project->toAbsolutePath(file);
    Graph* g = loadGraph(std::string(absolutePath.toUtf8().data()),progress);
    rootIds[entry] = g;

    addGraph(g);
  }

  QDir::setCurrent(QFileInfo(project->projectFile()).absolutePath());

  return rootIds;
}
QMap<tlp::Graph*,QString> GraphHierarchiesModel::writeProject(tlp::TulipProject *project, tlp::PluginProgress *progress) {
  QMap<tlp::Graph*,QString> rootIds;

  project->removeAllDir(GRAPHS_PATH);
  project->mkpath(GRAPHS_PATH);
  int i=0;
  foreach(tlp::Graph* g, _graphs) {
    rootIds[g] = QString::number(i);
    QString folder = GRAPHS_PATH + "/" + QString::number(i++) + "/";
    project->mkpath(folder);
    tlp::saveGraph(g,project->toAbsolutePath(folder + "graph.tlp").toStdString(),progress);
  }
  foreach(GraphNeedsSavingObserver* observer, _saveNeeded)
  observer->saved();
  return rootIds;
}

// Model related
QModelIndex GraphHierarchiesModel::index(int row, int column, const QModelIndex &parent) const {
  if (row < 0)
    return QModelIndex();

  Graph *g = NULL;

  if (parent.isValid())
    g = reinterpret_cast<Graph*>(parent.internalPointer())->getNthSubGraph(row);
  else if (row < _graphs.size())
    g = _graphs[row];

  if (g == NULL) {
    return QModelIndex();
  }

  return createIndex(row,column,g);
}

QModelIndex GraphHierarchiesModel::parent(const QModelIndex &child) const {
  if (!child.isValid())
    return QModelIndex();

  Graph* childGraph = reinterpret_cast<Graph*>(child.internalPointer());

  if (childGraph == NULL || _graphs.contains(childGraph) || childGraph->getSuperGraph() == childGraph) {
    return QModelIndex();
  }

  int row=0;
  Graph *parent = childGraph->getSuperGraph();

  if (_graphs.contains(parent))
    row=_graphs.indexOf(parent);
  else {
    Graph *ancestor = parent->getSuperGraph();

    for (unsigned int i=0; i<ancestor->numberOfSubGraphs(); i++) {
      if (ancestor->getNthSubGraph(i) == parent) {
        break;
      }

      row++;
    }
  }

  return createIndex(row,0,parent);
}

int GraphHierarchiesModel::rowCount(const QModelIndex &parent) const {
  if (!parent.isValid())
    return _graphs.size();

  if (parent.column() != 0)
    return 0;

  Graph* parentGraph = reinterpret_cast<Graph*>(parent.internalPointer());

  return parentGraph->numberOfSubGraphs();
}

int GraphHierarchiesModel::columnCount(const QModelIndex&) const {
  return 4;
}

bool GraphHierarchiesModel::setData(const QModelIndex &index, const QVariant &value, int role) {
  if (index.column() == NAME_SECTION) {
    Graph *graph = reinterpret_cast<Graph*>(index.internalPointer());
    graph->setName(std::string(value.toString().toUtf8().data()));
    return true;
  }

  return QAbstractItemModel::setData(index,value,role);
}

QVariant GraphHierarchiesModel::data(const QModelIndex &index, int role) const {

  if (!index.isValid())
    return QVariant();

  Graph *graph = reinterpret_cast<Graph*>(index.internalPointer());

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    if (index.column() == NAME_SECTION)
      return generateName(graph);
    else if (index.column() == ID_SECTION)
      return graph->getId();
    else if (index.column() == NODES_SECTION)
      return graph->numberOfNodes();
    else if (index.column() == EDGES_SECTION)
      return graph->numberOfEdges();
  }

  else if (role == Qt::ToolTipRole) {
    return QString("<table><tr><td>%1</td></tr><tr><td>Id = %2, Nodes = %3, Edges= %4</tr></td></table>").arg(generateName(graph)).arg(graph->getId()).arg(graph->numberOfNodes()).arg(graph->numberOfEdges());
  }

  else if (role == GraphRole) {
    return QVariant::fromValue<Graph*>(reinterpret_cast<Graph*>(index.internalPointer()));
  }

  else if (role == Qt::TextAlignmentRole && index.column() != NAME_SECTION)
    return Qt::AlignCenter;

  else if (role == Qt::FontRole) {
    QFont f;

    if (graph == _currentGraph)
      f.setBold(true);

    return f;
  }

  return QVariant();
}

QVariant GraphHierarchiesModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation == Qt::Horizontal) {
    if (role == Qt::DisplayRole) {
      if (section == NAME_SECTION)
        return trUtf8("Name");
      else if (section == ID_SECTION)
        return trUtf8("Id");
      else if (section == NODES_SECTION)
        return trUtf8("Nodes");
      else if (section == EDGES_SECTION)
        return trUtf8("Edges");
    }

    else if (role == Qt::TextAlignmentRole && section != NAME_SECTION)
      return Qt::AlignCenter;
  }

  return TulipModel::headerData(section,orientation,role);
}

Qt::ItemFlags GraphHierarchiesModel::flags(const QModelIndex &index) const {
  Qt::ItemFlags result = QAbstractItemModel::flags(index);

  if (index.column() == 0)
    result |= Qt::ItemIsEditable | Qt::ItemIsDragEnabled;

  return result;
}

QMimeData* GraphHierarchiesModel::mimeData(const QModelIndexList &indexes) const {
  QSet<Graph*> graphs;

  foreach(QModelIndex index, indexes) {
    Graph *g = data(index,GraphRole).value<Graph*>();

    if (g != NULL)
      graphs.insert(g);
  }

  GraphMimeType* result = new GraphMimeType();

  //every current implementation uses a single graph, so we do not have a graphmim with multiple graphs.
  foreach(Graph* g,graphs) {
    result->setGraph(g);
  }

  return result;
}

// Graphs collection
QString GraphHierarchiesModel::generateName(tlp::Graph *graph) const {
  std::string name = graph->getName();

  if (name.empty()) {
    stringstream ss;
    ss << "graph_" << graph->getId();
    name = ss.str();
    graph->setName(name);
  }

  return tlp::tlpStringToQString(name);
}

void GraphHierarchiesModel::setCurrentGraph(tlp::Graph *g) {

  bool inHierarchy = false;
  foreach(Graph *i,_graphs) {
    if (i->isDescendantGraph(g) || g == i) {
      inHierarchy = true;
      break;
    }
  }

  if (!inHierarchy)
    return;

  Graph* oldGraph = _currentGraph;
  _currentGraph = g;

  if (oldGraph != NULL && oldGraph != _currentGraph) {
    QModelIndex oldRow1 = indexOf(oldGraph);
    QModelIndex oldRow2 = createIndex(oldRow1.row(),columnCount()-1);
    emit dataChanged(oldRow1,oldRow2);
  }

  if (_currentGraph != NULL) {
    QModelIndex newRow1 = indexOf(_currentGraph);
    QModelIndex newRow2 = createIndex(newRow1.row(),columnCount()-1);
    emit dataChanged(newRow1,newRow2);
  }

  emit currentGraphChanged(g);
}

Graph *GraphHierarchiesModel::currentGraph() const {
  return _currentGraph;
}

void GraphHierarchiesModel::initIndexCache(tlp::Graph *root) {
  Graph *sg = NULL;
  int i = 0;
  forEach(sg, root->getSubGraphs()) {
    _indexCache[sg] = createIndex(i++, 0, sg);
    initIndexCache(sg);
  }
}

static void addListenerToWholeGraphHierarchy(Graph *root, Observable *listener) {
  Graph *sg = NULL;
  forEach(sg, root->getSubGraphs()) {
    addListenerToWholeGraphHierarchy(sg, listener);
  }
  root->addListener(listener);
  root->addObserver(listener);
}

void GraphHierarchiesModel::addGraph(tlp::Graph *g) {
  if (_graphs.contains(g) || g == NULL)
    return;

  Graph *i;
  foreach(i,_graphs) {
    if (i->isDescendantGraph(g))
      return;
  }

  beginInsertRows(QModelIndex(),rowCount(),rowCount());

  _saveNeeded[g] = new GraphNeedsSavingObserver(g);

  _graphs.push_back(g);
  g->getProperty<ColorProperty>("viewColor")->setMetaValueCalculator(&vColorCalc);
  g->getProperty<StringProperty>("viewLabel")->setMetaValueCalculator(&vLabelCalc);
  g->getProperty<DoubleProperty>("viewBorderWidth")->setMetaValueCalculator(&vWidthCalc);

  // listen events on the whole hierarchy
  // in order to keep track of subgraphs names, number of nodes and edges
  addListenerToWholeGraphHierarchy(g, this);

  if (_graphs.size() == 1)
    setCurrentGraph(g);

  endInsertRows();
  initIndexCache(g);
}

void GraphHierarchiesModel::removeGraph(tlp::Graph *g) {
  if (_graphs.contains(g)) {
    int pos = _graphs.indexOf(g);
    beginRemoveRows(QModelIndex(),pos,pos);
    _graphs.removeAll(g);
    GraphNeedsSavingObserver *s= _saveNeeded.take(g);
    delete s;
    endRemoveRows();

    if (_currentGraph == g) {
      if (_graphs.empty()) {
        _currentGraph = NULL;
        emit currentGraphChanged(_currentGraph);
      }
      else
        setCurrentGraph(_graphs[0]);
    }
  }
}

// Observation
void GraphHierarchiesModel::treatEvent(const Event &e) {
  Graph *g = dynamic_cast<tlp::Graph *>(e.sender());
  assert(g);

  if (e.type() == Event::TLP_DELETE && _graphs.contains(g)) { // A root graph has been deleted
    int pos = _graphs.indexOf(g);
    beginRemoveRows(QModelIndex(),pos,pos);

    _graphs.removeAll(g);
    GraphNeedsSavingObserver *s= _saveNeeded.take(g);
    delete s;

    if (_currentGraph == g) {
      if (_graphs.empty())
        _currentGraph = NULL;
      else
        _currentGraph = _graphs[0];

      emit currentGraphChanged(_currentGraph);
    }

    endRemoveRows();
  }
  else if (e.type() == Event::TLP_MODIFICATION) {
    const GraphEvent *ge = dynamic_cast<const tlp::GraphEvent *>(&e);

    if (!ge)
      return;

    if (_graphs.contains(ge->getGraph()->getRoot())) {


      if (ge->getType() == GraphEvent::TLP_AFTER_ADD_DESCENDANTGRAPH) {
        // that event must only be treated on a root graph
        if (ge->getGraph() != ge->getGraph()->getRoot()) {
          return;
        }

        const Graph* sg = ge->getSubGraph();

        Graph* parentGraph = sg->getSuperGraph();

#ifndef NDEBUG
        QModelIndex parentIndex = indexOf(parentGraph);

        assert(parentIndex.isValid());
#endif

        // update index cache for subgraphs of parent graph and added sub-graphs
        Graph* sg2 = NULL;

        int i = 0;

        forEach(sg2, parentGraph->getSubGraphs()) {
          _indexCache[sg2] = createIndex(i++, 0, sg2);
        }

        i = 0;
        forEach(sg2, sg->getSubGraphs()) {
          _indexCache[sg2] = createIndex(i++, 0, sg2);

        }

        sg->addListener(this);
        sg->addObserver(this);

        // insert the parent graph in the graphs changed set
        // in order to update associated tree views, displaying the graphs hierarchies,
        // when the treatEvents method is called
        _graphsChanged.insert(parentGraph);

      }
      else if (ge->getType() == GraphEvent::TLP_AFTER_DEL_DESCENDANTGRAPH) {
        // that event must only be treated on a root graph
        if (ge->getGraph() != ge->getGraph()->getRoot()) {
          return;
        }

        const Graph* sg = ge->getSubGraph();

        Graph* parentGraph = sg->getSuperGraph();

        QModelIndex index = indexOf(sg);

        assert(index.isValid());

#ifndef NDEBUG
        QModelIndex parentIndex = indexOf(parentGraph);

        assert(parentIndex.isValid());
#endif

        // update index cache for subgraphs of parent graph
        Graph* sg2 = NULL;

        int i = 0;

        forEach(sg2, parentGraph->getSubGraphs()) {
          _indexCache[sg2] = createIndex(i++, 0, sg2);
        }

        // prevent dangling pointer to remain in the persistent indexes
        _indexCache.remove(sg);
        changePersistentIndex(index, QModelIndex());

        sg->removeListener(this);
        sg->removeObserver(this);

        // insert the parent graph in the graphs changed set
        // in order to update associated tree views, displaying the graphs hierarchies,
        // when the treatEvents method is called
        _graphsChanged.insert(parentGraph);

        // remove the subgraph from the graphs changed set
        // as no update will be required for it in the associated tree views
        _graphsChanged.remove(sg);

        if (currentGraph() == sg) {
          setCurrentGraph(parentGraph);
          setCurrentGraph(parentGraph);
        }

      }
      else if (ge->getType() == GraphEvent::TLP_ADD_NODE || ge->getType() == GraphEvent::TLP_ADD_NODES ||
               ge->getType() == GraphEvent::TLP_DEL_NODE) {
        const Graph *graph = ge->getGraph();
        // row representing the graph in the associated tree views has to be updated
        _graphsChanged.insert(graph);

      }
      else if (ge->getType() == GraphEvent::TLP_ADD_EDGE || ge->getType() == GraphEvent::TLP_ADD_EDGES ||
               ge->getType() == GraphEvent::TLP_DEL_EDGE) {
        const Graph *graph = ge->getGraph();
        // row representing the graph in the associated tree views has to be updated
        _graphsChanged.insert(graph);
      }
    }
  }
  else if (e.type() == Event::TLP_INFORMATION) {
    const GraphEvent *ge = dynamic_cast<const tlp::GraphEvent *>(&e);

    if (!ge)
      return;

    if (ge->getType() == GraphEvent::TLP_AFTER_SET_ATTRIBUTE && ge->getAttributeName() == "name") {
      const Graph *graph = ge->getGraph();
      // TLP_INFORMATION events are only sent to listeners so we are forced to emit the dataChanged signal
      // here as the treatEvents method will not be called afterwards in that particular case
      QModelIndex graphIndex = indexOf(graph);
      QModelIndex graphEdgesIndex = graphIndex.sibling(graphIndex.row(), EDGES_SECTION);
      emit dataChanged(graphIndex, graphEdgesIndex);
    }

  }
}

void GraphHierarchiesModel::treatEvents(const std::vector<tlp::Event> &) {

  if (_graphsChanged.isEmpty()) {
    return;
  }

  // update the rows associated to modified graphs (number of subgraphs/nodes/edges has changed)
  // in the associated tree views

  emit layoutAboutToBeChanged();

  const Graph *graph = NULL;
  foreach (graph, _graphsChanged) {
    QModelIndex graphIndex = indexOf(graph);
    QModelIndex graphEdgesIndex = graphIndex.sibling(graphIndex.row(), EDGES_SECTION);
    emit dataChanged(graphIndex, graphEdgesIndex);
  }

  emit layoutChanged();

  _graphsChanged.clear();
}
