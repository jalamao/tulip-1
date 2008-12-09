#include "tulip/RenderingParametersDialog.h"

#include <QtGui/QHeaderView>
#include <QtGui/QColorDialog>

#include "tulip/GWOverviewWidget.h"
#include "tulip/GlMainWidget.h"
#include "tulip/GlMainView.h"

using namespace std;

namespace tlp {

  RenderingParametersDialog::RenderingParametersDialog(QWidget* parent) : QDialog(parent) {
    setupUi(this);

    holdUpdateView=false;

    treeWidget->header()->resizeSection(0,205);
    treeWidget->header()->resizeSection(1,70);
    treeWidget->header()->resizeSection(2,70);
  }

  void RenderingParametersDialog::windowActivationChange(bool oldActive) {
    if (!oldActive)
      buttonClose->setFocus();
  }

  void RenderingParametersDialog::setGlMainView(GlMainView *view){
    mainView=view;

    GlGraphRenderingParameters param = mainView->getGlMainWidget()->getScene()->getGlGraphComposite()->getRenderingParameters();

    holdUpdateView=true;
    arrows->setChecked( param.isViewArrow());
    colorInterpolation->setChecked( param.isEdgeColorInterpolate());
    sizeInterpolation->setChecked( param.isEdgeSizeInterpolate());
    ordering->setChecked( param.isElementOrdered());
    orthogonal->setChecked(mainView->getGlMainWidget()->getScene()->isViewOrtho());
    edge3D->setChecked( param.isEdge3D());
    Color inColor = mainView->getGlMainWidget()->getScene()->getBackgroundColor();
    setBackgroundColor(QColor(inColor[0],inColor[1],inColor[2]));
    fonts->setCurrentIndex(param.getFontsType());
    density->setValue(param.getLabelsBorder());

    holdUpdateView=false;

    attachMainWidget(view->getGlMainWidget());
  }

  void RenderingParametersDialog::updateView() {
    if(holdUpdateView)
      return;

    GlGraphRenderingParameters param = mainView->getGlMainWidget()->getScene()->getGlGraphComposite()->getRenderingParameters();

    param.setViewArrow(arrows->isChecked());
    param.setEdgeColorInterpolate(colorInterpolation->isChecked());
    param.setEdgeSizeInterpolate(sizeInterpolation->isChecked());
    param.setElementOrdered(ordering->isChecked());
    mainView->getGlMainWidget()->getScene()->setViewOrtho(orthogonal->isChecked());
    param.setEdge3D(edge3D->isChecked());
    param.setFontsType(fonts->currentIndex());
    QColor outColor = background->palette().color(QPalette::Button);
    mainView->getGlMainWidget()->getScene()->setBackgroundColor(Color(outColor.red(),outColor.green(),outColor.blue()));
    param.setLabelsBorder(density->value());

    mainView->getGlMainWidget()->getScene()->getGlGraphComposite()->setRenderingParameters(param);
    mainView->draw();
  }

  void RenderingParametersDialog::backColor() {
    setBackgroundColor(QColorDialog::getColor(background->palette().color(QPalette::Button), this));
    updateView();
  }

  void RenderingParametersDialog::setBackgroundColor(QColor tmp) {
    if (tmp.isValid()) {
      QPalette palette;
      palette.setColor(QPalette::Button, tmp);
      int h,s,v;
      tmp.getHsv(&h, &s, &v);
      if (v < 128)
	palette.setColor(QPalette::ButtonText, QColor(255, 255, 255));
      else
	palette.setColor(QPalette::ButtonText, QColor(0, 0, 0));
      background->setPalette(palette);
    }
  }


  //=============================================================================
void RenderingParametersDialog::attachMainWidget(GlMainWidget* graphWidget) {
  treeWidget->invisibleRootItem()->takeChildren();

  observedMainWidget=graphWidget;

  vector<pair<string, GlLayer*> >* layers=graphWidget->getScene()->getLayersList();
  if(layers->size()!=0) {
    for(vector<pair<string, GlLayer*> >::iterator it=layers->begin();it!=layers->end();++it) {
      addLayer(graphWidget->getScene(),(*it).first,(*it).second);
    }
  }
  treeWidget->expandAll();
  connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)),this, SLOT(checkBoxClicked(QTreeWidgetItem*, int)));
}
//=============================================================================
void RenderingParametersDialog::addLayer(GlScene* scene, const string& name, GlLayer* layer){
  QTreeWidgetItem *item=new QTreeWidgetItem(treeWidget,QStringList(name.c_str()));
  item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  if(layer->isVisible())
    item->setCheckState(1,Qt::Checked);
  else
    item->setCheckState(1,Qt::Unchecked);
  addComposite(layer->getComposite(),item);
  treeWidget->expandAll();
}
//=============================================================================
void RenderingParametersDialog::addComposite(GlComposite *composite,QTreeWidgetItem *parent) {
  map<string, GlSimpleEntity*> *entities=composite->getDisplays();
  for(map<string, GlSimpleEntity*>::iterator it=entities->begin();it!=entities->end();++it) {
    QTreeWidgetItem* item=new QTreeWidgetItem(parent,QStringList((*it).first.c_str()));
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    if((*it).second->isVisible())
      item->setCheckState(1,Qt::Checked);
    else
      item->setCheckState(1,Qt::Unchecked);

    GlComposite *glComposite = dynamic_cast<GlComposite *> ((*it).second);
    GlGraphComposite *glGraphComposite = dynamic_cast<GlGraphComposite *> ((*it).second);
    if(glGraphComposite) {
      createGraphCompositeItem(glGraphComposite,item);
    }else if(glComposite) {
      addComposite(glComposite,item);
    }

    if(!glGraphComposite) {
      if((*it).second->getStencil()!=0xFFFF)
	item->setCheckState(2,Qt::Checked);
      else
	item->setCheckState(2,Qt::Unchecked);
    }
  }
}
//=============================================================================
void RenderingParametersDialog::createGraphCompositeItem(GlGraphComposite *glGraphComposite,QTreeWidgetItem *item) {
  //Nodes
  QTreeWidgetItem* nodes=new QTreeWidgetItem(item,QStringList("Nodes"));
  nodes->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  if(glGraphComposite->isDisplayNodes())
    nodes->setCheckState(1,Qt::Checked);
  else
    nodes->setCheckState(1,Qt::Unchecked);
  if(glGraphComposite->getNodesStencil()!=0xFFFF)
    nodes->setCheckState(2,Qt::Checked);
  else
    nodes->setCheckState(2,Qt::Unchecked);
  //Meta-Nodes
  QTreeWidgetItem* metaNodes=new QTreeWidgetItem(item,QStringList("Meta-Nodes"));
  metaNodes->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  if(glGraphComposite->isDisplayMetaNodes())
    metaNodes->setCheckState(1,Qt::Checked);
  else
    metaNodes->setCheckState(1,Qt::Unchecked);
  if(glGraphComposite->getMetaNodesStencil()!=0xFFFF)
    metaNodes->setCheckState(2,Qt::Checked);
  else
    metaNodes->setCheckState(2,Qt::Unchecked);
  //Edges
  QTreeWidgetItem* edges=new QTreeWidgetItem(item,QStringList("Edges"));
  edges->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  if(glGraphComposite->isDisplayEdges())
    edges->setCheckState(1,Qt::Checked);
  else
    edges->setCheckState(1,Qt::Unchecked);
  if(glGraphComposite->getEdgesStencil()!=0xFFFF)
    edges->setCheckState(2,Qt::Checked);
  else
    edges->setCheckState(2,Qt::Unchecked);
  //Nodes Label
  QTreeWidgetItem* nodesLabel=new QTreeWidgetItem(item,QStringList("Nodes Label"));
  nodesLabel->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  if(glGraphComposite->isDisplayNodesLabel())
    nodesLabel->setCheckState(1,Qt::Checked);
  else
    nodesLabel->setCheckState(1,Qt::Unchecked);
  if(glGraphComposite->getNodesLabelStencil()!=0xFFFF)
    nodesLabel->setCheckState(2,Qt::Checked);
  else
    nodesLabel->setCheckState(2,Qt::Unchecked);
  //Meta-Nodes Label
  QTreeWidgetItem* metaNodesLabel=new QTreeWidgetItem(item,QStringList("Meta-Nodes Label"));
  metaNodesLabel->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  if(glGraphComposite->isDisplayMetaNodesLabel())
    metaNodesLabel->setCheckState(1,Qt::Checked);
  else
    metaNodesLabel->setCheckState(1,Qt::Unchecked);
  if(glGraphComposite->getMetaNodesLabelStencil()!=0xFFFF)
    metaNodesLabel->setCheckState(2,Qt::Checked);
  else
    metaNodesLabel->setCheckState(2,Qt::Unchecked);
  //Edges Label
  QTreeWidgetItem* edgesLabel=new QTreeWidgetItem(item,QStringList("Edges Label"));
  edgesLabel->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  if(glGraphComposite->isDisplayEdgesLabel())
    edgesLabel->setCheckState(1,Qt::Checked);
  else
    edgesLabel->setCheckState(1,Qt::Unchecked);
  if(glGraphComposite->getEdgesLabelStencil()!=0xFFFF)
    edgesLabel->setCheckState(2,Qt::Checked);
  else
    edgesLabel->setCheckState(2,Qt::Unchecked);
  //Selected Nodes
  QTreeWidgetItem* selectedNodes=new QTreeWidgetItem(item,QStringList("Selected nodes"));
  selectedNodes->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  if(glGraphComposite->getSelectedNodesStencil()!=0xFFFF)
    selectedNodes->setCheckState(2,Qt::Checked);
  else
    selectedNodes->setCheckState(2,Qt::Unchecked);
  //Selected Nodes
  QTreeWidgetItem* selectedMetaNodes=new QTreeWidgetItem(item,QStringList("Selected meta-nodes"));
  selectedMetaNodes->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  if(glGraphComposite->getSelectedMetaNodesStencil()!=0xFFFF)
    selectedMetaNodes->setCheckState(2,Qt::Checked);
  else
    selectedMetaNodes->setCheckState(2,Qt::Unchecked);
  //Selected Edges
  QTreeWidgetItem* selectedEdges=new QTreeWidgetItem(item,QStringList("Selected edges"));
  selectedEdges->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
  if(glGraphComposite->getSelectedEdgesStencil()!=0xFFFF)
    selectedEdges->setCheckState(2,Qt::Checked);
  else
    selectedEdges->setCheckState(2,Qt::Unchecked);
}
//=============================================================================
void RenderingParametersDialog::updateLayer(const string& name,GlLayer *layer) {
  QTreeWidgetItem* root=treeWidget->invisibleRootItem();
  for(int i=0;i<root->childCount();i++) {
    QTreeWidgetItem *child=root->child(i);
    if(child->data(0,0).toString().toStdString()==name) {
      child->takeChildren();
      addComposite(layer->getComposite(),child);
      break;
    }
  }
  treeWidget->expandAll();
}
//=============================================================================
void RenderingParametersDialog::delLayer(GlScene*, const string&, GlLayer*){
  assert(false);
}
//=============================================================================
void RenderingParametersDialog::checkBoxClicked(QTreeWidgetItem* item, int column) {
  if(column!=1 && column!=2)
    return;

  if(column==1) {
    if(!item->parent()) {
      //Layer
      if(!observedMainWidget->getScene()->getLayer(item->data(0,0).toString().toStdString()))
	return;
      if(item->checkState(column)==Qt::Unchecked){
	observedMainWidget->getScene()->getLayer(item->data(0,0).toString().toStdString())->setVisible(false);
      }else{
	observedMainWidget->getScene()->getLayer(item->data(0,0).toString().toStdString())->setVisible(true);
      }
    }else{
      QList<string> hierarchie;
      buildHierarchie(item,hierarchie);
      GlLayer* layer=observedMainWidget->getScene()->getLayer(hierarchie[0]);
      if(!layer)
	return;
      GlSimpleEntity* entity=layer->getComposite();
      bool isGraphComposite=false;
      for(QList<string>::iterator it=(++hierarchie.begin());it!=hierarchie.end();++it) {
	GlSimpleEntity* newEntity=(GlSimpleEntity*)((GlComposite*)entity)->findGlEntity(*it);
	if(newEntity)
	  entity=newEntity;
	else
	  isGraphComposite=true;
      }

      if(!isGraphComposite) {
	if(item->checkState(column)==Qt::Unchecked){
	  entity->setVisible(false);
	}else{
	  entity->setVisible(true);
	}
      }else{
	GlGraphComposite *composite=(GlGraphComposite*) entity;
	if(item->data(0,0).toString().toStdString()=="Nodes")
	  composite->setDisplayNodes(item->checkState(column)==Qt::Checked);
	else if (item->data(0,0).toString().toStdString()=="Edges")
	  composite->setDisplayEdges(item->checkState(column)==Qt::Checked);
	else if (item->data(0,0).toString().toStdString()=="Meta-Nodes")
	  composite->setDisplayMetaNodes(item->checkState(column)==Qt::Checked);
	else if (item->data(0,0).toString().toStdString()=="Nodes Label")
	  composite->setDisplayNodesLabel(item->checkState(column)==Qt::Checked);
	else if (item->data(0,0).toString().toStdString()=="Meta-Nodes Label")
	  composite->setDisplayMetaNodesLabel(item->checkState(column)==Qt::Checked);
	else if (item->data(0,0).toString().toStdString()=="Edges Label")
	  composite->setDisplayEdgesLabel(item->checkState(column)==Qt::Checked);
	else
	  assert(false);
      }
    }
  }else{
    QList<string> hierarchie;
    buildHierarchie(item,hierarchie);
    GlLayer* layer=observedMainWidget->getScene()->getLayer(hierarchie[0]);
    if(!layer)
      return;
    GlSimpleEntity* entity=layer->getComposite();
    bool isGraphComposite=false;
    for(QList<string>::iterator it=(++hierarchie.begin());it!=hierarchie.end();++it) {
      GlSimpleEntity* newEntity=(GlSimpleEntity*)((GlComposite*)entity)->findGlEntity(*it);
      if(newEntity)
	entity=newEntity;
      else
	isGraphComposite=true;
    }

    if(!isGraphComposite) {
      if(item->checkState(column)==Qt::Unchecked){
	entity->setStencil(0xFFFF);
      }else{
	entity->setStencil(2);
      }
    }else{
      GlGraphComposite *composite=(GlGraphComposite*) entity;
      int value;
      if(item->checkState(column)==Qt::Checked)
	value=2;
      else
	value=0xFFFF;
      if(item->data(0,0).toString().toStdString()=="Selected nodes")
	composite->setSelectedNodesStencil(value);
      if(item->data(0,0).toString().toStdString()=="Selected meta-nodes")
	composite->setSelectedMetaNodesStencil(value);
      if(item->data(0,0).toString().toStdString()=="Selected edges")
	composite->setSelectedEdgesStencil(value);
      else if(item->data(0,0).toString().toStdString()=="Nodes")
	composite->setNodesStencil(value);
      else if (item->data(0,0).toString().toStdString()=="Edges")
	  composite->setEdgesStencil(value);
      else if (item->data(0,0).toString().toStdString()=="Meta-Nodes")
	composite->setMetaNodesStencil(value);
      else if (item->data(0,0).toString().toStdString()=="Nodes Label")
	composite->setNodesLabelStencil(value);
      else if (item->data(0,0).toString().toStdString()=="Meta-Nodes Label")
	composite->setMetaNodesLabelStencil(value);
      else if (item->data(0,0).toString().toStdString()=="Edges Label")
	composite->setEdgesLabelStencil(value);
    }
  }

  observedMainWidget->draw();
}
//=============================================================================
void RenderingParametersDialog::buildHierarchie(QTreeWidgetItem *item,QList<string>& hierarchie) {
  if(item->parent())
    buildHierarchie(item->parent(),hierarchie);
  hierarchie.push_back(item->data(0,0).toString().toStdString());
}

}
