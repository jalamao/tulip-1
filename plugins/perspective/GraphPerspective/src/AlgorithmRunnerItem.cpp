#include "AlgorithmRunnerItem.h"

#include "ui_AlgorithmRunnerItem.h"

#include <QtGui/QMouseEvent>

#include "GraphPerspective.h"
#include <tulip/GraphTest.h>
#include <tulip/TulipMimes.h>
#include <tulip/TulipItemDelegate.h>
#include <tulip/ParameterListModel.h>

using namespace tlp;

AlgorithmRunnerItem::AlgorithmRunnerItem(QString pluginName, QWidget *parent): QWidget(parent), _ui(new Ui::AlgorithmRunnerItem), _pluginName(pluginName), _localMode(true) {
  _ui->setupUi(this);
  connect(_ui->favoriteCheck,SIGNAL(toggled(bool)),this,SIGNAL(favorized(bool)));
  _ui->algorithmName->setText(pluginName);
  _ui->parameters->setVisible(false);
  _ui->parameters->setItemDelegate(new TulipItemDelegate);
  setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Maximum);
}

AlgorithmRunnerItem::~AlgorithmRunnerItem() {
  delete _ui;
}

void AlgorithmRunnerItem::setGraph(Graph* g) {
  _graph = g;

  if (_ui->parameters->model() != NULL)
    _ui->parameters->setModel(NULL);

  if (_ui->parameters->isVisible())
    initModel();
}

void AlgorithmRunnerItem::setData(const DataSet &data) {
  initModel();
  ParameterListModel* model = static_cast<ParameterListModel*>(_ui->parameters->model());
  model->setParametersValues(data);
}

QString AlgorithmRunnerItem::name() const {
  return _pluginName;
}

template<typename PROP>
void asLocal(QVariant var, DataSet& data, Graph* g) {
  if (var.userType() == qMetaTypeId<PROP*>()) {
    PROP* local = g->getLocalProperty<PROP>(var.value<PROP*>()->getName());
    data.set("result",local);
  }
}
void copyToLocal(DataSet& data, Graph* g) {
  if (!data.exist("result"))
    return;

  DataType* d = data.getData("result");
  QVariant var = TulipMetaTypes::dataTypeToQvariant(d,"");
  asLocal<DoubleProperty>(var,data,g);
  asLocal<IntegerProperty>(var,data,g);
  asLocal<LayoutProperty>(var,data,g);
  asLocal<SizeProperty>(var,data,g);
  asLocal<ColorProperty>(var,data,g);
  asLocal<BooleanProperty>(var,data,g);
  asLocal<StringProperty>(var,data,g);
}
void AlgorithmRunnerItem::run(Graph *g) {
  initModel();

  if (g == NULL)
    g=_graph;

  if (g == NULL) {
    qCritical() << name() << trUtf8(": No graph selected");
    return;
  }

  Observable::holdObservers();
  DataSet dataSet = static_cast<ParameterListModel*>(_ui->parameters->model())->parametersValues();

  if (_localMode)
    copyToLocal(dataSet, g);

  std::string errorMessage;
  PluginProgress* progress = Perspective::instance()->progress();
  bool result = g->applyAlgorithm(_pluginName.toStdString(),errorMessage,&dataSet,progress);
  delete progress;

  if (!result)
    qCritical() << name() << ": " << errorMessage.c_str();

  afterRun(g,dataSet);

  Observable::unholdObservers();
}

void AlgorithmRunnerItem::setLocalMode(bool m) {
  _localMode = m;
}

void AlgorithmRunnerItem::mousePressEvent(QMouseEvent *ev) {
  if (ev->button() == Qt::LeftButton)
    _dragStartPosition = ev->pos();
}

void AlgorithmRunnerItem::mouseMoveEvent(QMouseEvent *ev) {
  if (!(ev->buttons() & Qt::LeftButton) ||
      (ev->pos() - _dragStartPosition).manhattanLength() < QApplication::startDragDistance())
    return;

  QDrag *drag = new QDrag(this);
  const Plugin* p = PluginLister::pluginInformations(_pluginName.toStdString().c_str());
  QPixmap icon(QPixmap(p->icon().c_str()).scaled(64,64));
  delete p;
  QFont f;
  f.setBold(true);
  QFontMetrics metrics(f);
  int textHeight = metrics.boundingRect(0,0,icon.width(),INT_MAX,Qt::AlignTop | Qt::AlignHCenter | Qt::TextWordWrap,_pluginName).height();
  QPixmap pix(icon.width()+textHeight,icon.height()+textHeight);
  pix.fill(Qt::white);
  QPainter painter(&pix);
  painter.drawPixmap(pix.width()/2 - icon.width()/2,0,icon.width(),icon.height(),icon);
  painter.setFont(f);
  painter.drawText(0,icon.height(),pix.width(),pix.height()-icon.height(),Qt::AlignCenter | Qt::AlignHCenter | Qt::TextWordWrap,_pluginName);
  painter.setBrush(Qt::transparent);
  painter.setPen(QColor(169, 169, 169));
  painter.drawRect(0,0,pix.width()-1,pix.height()-1);
  drag->setPixmap(pix);

  initModel();
  AlgorithmMimeType* mimeData = new AlgorithmMimeType(name(),static_cast<ParameterListModel*>(_ui->parameters->model())->parametersValues());
  connect(mimeData,SIGNAL(mimeRun(tlp::Graph*,tlp::DataSet)),this,SLOT(afterRun(tlp::Graph*,tlp::DataSet)));
  drag->setMimeData(mimeData);
  drag->exec(Qt::CopyAction | Qt::MoveAction);
}

void AlgorithmRunnerItem::afterRun(Graph* g, tlp::DataSet dataSet) {
  if (PluginLister::instance()->pluginExists<LayoutAlgorithm>(name().toStdString()))
    Perspective::typedInstance<GraphPerspective>()->centerPanelsForGraph(g);
  else if (PluginLister::instance()->pluginExists<GraphTest>(name().toStdString())) {
    bool result = true;
    dataSet.get<bool>("result",result);
    std::string gname;
    g->getAttribute<std::string>("name",gname);
    qDebug() << gname.c_str() << " - " << name() << (result ? trUtf8(": sucess") : trUtf8(": failure"));
    // TODO : add a dialog here
  }
}

void AlgorithmRunnerItem::setFavorite(bool f) {
  _ui->favoriteCheck->setChecked(f);
}

tlp::DataSet AlgorithmRunnerItem::data() const {
  if (_ui->parameters->model() == NULL)
    return DataSet();

  return static_cast<ParameterListModel*>(_ui->parameters->model())->parametersValues();
}


void AlgorithmRunnerItem::initModel() {
  if (_ui->parameters->model() != NULL)
    return;

  ParameterListModel* model = new ParameterListModel(PluginLister::getPluginParameters(_pluginName.toStdString()),_graph,_ui->parameters);
  _ui->parameters->setModel(model);
  _ui->settingsButton->setEnabled(model->rowCount() > 0);
  int h = 10;

  for (int i=0; i<model->rowCount(); ++i)
    h += _ui->parameters->rowHeight(i);

  _ui->parameters->setMinimumSize(_ui->parameters->minimumSize().width(),h);
  _ui->parameters->setMaximumSize(_ui->parameters->maximumSize().width(),h);
}