#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <vector>

#ifdef  _WIN32
// compilation pb workaround
#include <windows.h>
#endif
#include <QtGui/qlistview.h>
#include <QtGui/qpushbutton.h>
#include <QtGui/qmessagebox.h>
#include <QtGui/qinputdialog.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtGui/qlabel.h>
#include <QtGui/qcolordialog.h>
#include <QtGui/qtabwidget.h>

#include <tulip/Graph.h>
#include <tulip/DoubleProperty.h>
#include <tulip/StringProperty.h>
#include <tulip/BooleanProperty.h>
#include <tulip/LayoutProperty.h>
#include <tulip/IntegerProperty.h>
#include <tulip/ColorProperty.h>
#include <tulip/SizeProperty.h>
#include <tulip/PropertyWidget.h>
#include <tulip/SGHierarchyWidget.h>
#include <tulip/GlMainWidget.h>
#include <tulip/ForEach.h>

#include "tulip/PropertyDialog.h"
#include "tulip/CopyPropertyDialog.h"
#include "tulip/ImportCSVDataWidget.h"
#include "tulip/ImportCSVDataDialog.h"

using namespace std;

namespace tlp {

  //==================================================================================================
  PropertyDialog::PropertyDialog(QWidget* parent)
    : QWidget(parent) {
    setupUi(this);
    _filterSelection=false;
    graph=0;
    connect(localProperties, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), SLOT(changePropertyName(QListWidgetItem *, QListWidgetItem *)));
    connect(inheritedProperties, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), SLOT(changePropertyName(QListWidgetItem *, QListWidgetItem *)));
    connect(removeButton , SIGNAL(clicked()) , SLOT(removeProperty()) );
    connect(newButton,SIGNAL(clicked()),SLOT(newProperty()));
    connect(cloneButton,SIGNAL(clicked()),SLOT(cloneProperty()));
    connect(radioAll,SIGNAL(clicked()),SLOT(populatePropertiesList()));
    connect(radioWork,SIGNAL(clicked()),SLOT(populatePropertiesList()));
    connect(radioView,SIGNAL(clicked()),SLOT(populatePropertiesList()));

  }
  //=================================================
  PropertyDialog::~PropertyDialog() {
  }
  //=================================================
  void PropertyDialog::changePropertyName(QListWidgetItem *item, QListWidgetItem *) {
    if (item==0) {
      editedProperty=0;
      return;
    }
    string erreurMsg;

    tableNodes->selectNodeOrEdge(true);
    tableEdges->selectNodeOrEdge(false);
    tableNodes->changeProperty(graph, item->text().toUtf8().data());
    tableEdges->changeProperty(graph, item->text().toUtf8().data());

    PropertyInterface *tmpProxy=graph->getProperty(item->text().toUtf8().data());
    editedProperty=tmpProxy;
    editedPropertyName=item->text().toUtf8().data();
    //propertyName->setText(item->text());

    if (graph->existLocalProperty(item->text().toUtf8().data()))
      inheritedProperties->clearSelection();
    else
      localProperties->clearSelection();

    tableEdges->update();
    tableNodes->update();
  }
  //=================================================
  void PropertyDialog::filterSelection(bool b) {
    _filterSelection= b;
    tableEdges->filterSelection(b);
    tableNodes->filterSelection(b);
    update();
  }
  //=================================================
  void PropertyDialog::update() {
    tableEdges->update();
    tableNodes->update();
  }
  //=================================================
  void PropertyDialog::selectNode(node n){}
  //=================================================
  void PropertyDialog::selectEdge(edge e){}
  //=================================================
  void PropertyDialog::setAllValue() {
    if (tabWidget->currentIndex()==0)
      tableNodes->setAll();
    else
      tableEdges->setAll();
    setAllButton->setDown(false);
  }
  //=================================================
  void PropertyDialog::setGraph(Graph *graph) {
    this->graph=graph;
    editedProperty=0;


    //Build the property list
    tableNodes->selectNodeOrEdge(true);
    tableEdges->selectNodeOrEdge(false);
    tableNodes->setGraph(graph);
    tableEdges->setGraph(graph);
    tableEdges->filterSelection(_filterSelection);
    tableNodes->filterSelection(_filterSelection);

    populatePropertiesList();
  }
  //=================================================
  void PropertyDialog::populatePropertiesList(){
    localProperties->clear();
    inheritedProperties->clear();

    if(graph==0) return;

    Iterator<string> *it=graph->getLocalProperties();
    while (it->hasNext()) {
      string tmp=it->next();

      if(radioView->isChecked() && tmp.substr(0,4)!="view")
        continue;
      if(radioWork->isChecked() && tmp.substr(0,4)=="view")
        continue;

      QListWidgetItem* tmpItem = new QListWidgetItem(localProperties);
      tmpItem->setText(QString::fromUtf8(tmp.c_str()));
    } delete it;
    it=graph->getInheritedProperties();
    while (it->hasNext()) {
      string tmp=it->next();

      if(radioView->isChecked() && tmp.substr(0,4)!="view")
        continue;
      if(radioWork->isChecked() && tmp.substr(0,4)=="view")
        continue;

      QListWidgetItem *tmpItem = new QListWidgetItem(inheritedProperties);
      tmpItem->setText(QString::fromUtf8(tmp.c_str()));
    } delete it;
  }
  //=================================================
  void PropertyDialog::newProperty() {
    if (!graph) return;
    QStringList lst;
    lst << "Color" << "Integer" << "Layout" << "Metric" << "Selection" << "Size" << "String" << "BooleanVector" << "ColorVector" << "CoordVector" << "DoubleVector" << "IntegerVector" << "SizeVector" << "StringVector";
    bool ok = false;
    QString res = QInputDialog::getItem(this, "Property type",
					"Please select the property type",
					lst, 3, false, &ok);
    if ( ok ) {
      QString text = QInputDialog::getText(this, "Property name",
					   "Please enter the property name",
					   QLineEdit::Normal, QString::null,
					   &ok);
      if(ok && text==""){
        ok=false;
        QMessageBox::warning(this, "Fail to create property",
                             "You can't create a property with empty name",
                             QMessageBox::Ok,
                             QMessageBox::Ok);
      }
      string textString(text.toUtf8().data());
      if(ok && graph->existLocalProperty(textString)) {
        ok = false;
        QMessageBox::warning(this, "Fail to create property",
                             "A property with same name already exist",
                           QMessageBox::Ok,
                           QMessageBox::Ok);
      }
      if (ok) {
        string erreurMsg;
        graph->push();
	char* resChars = res.toUtf8().data();
        if (strcmp(resChars,"Selection")==0)
	  graph->getLocalProperty<BooleanProperty>(textString);
        if (strcmp(resChars,"Metric")==0)
	  graph->getLocalProperty<DoubleProperty>(textString);
        if (strcmp(resChars,"Layout")==0)
	  graph->getLocalProperty<LayoutProperty>(textString);
        if (strcmp(resChars,"String")==0)
	  graph->getLocalProperty<StringProperty>(textString);
        if (strcmp(resChars,"Integer")==0)
	  graph->getLocalProperty<IntegerProperty>(textString);
        if (strcmp(resChars,"Size")==0)
	  graph->getLocalProperty<SizeProperty>(textString);
        if (strcmp(resChars,"Color")==0)
	  graph->getLocalProperty<ColorProperty>(textString);
        if (strcmp(resChars,"BooleanVector")==0)
	  graph->getLocalProperty<BooleanVectorProperty>(textString);
        if (strcmp(resChars,"DoubleVector")==0)
	  graph->getLocalProperty<DoubleVectorProperty>(textString);
        if (strcmp(resChars,"CoordVector")==0)
	  graph->getLocalProperty<CoordVectorProperty>(textString);
        if (strcmp(resChars,"StringVector")==0)
	  graph->getLocalProperty<StringVectorProperty>(textString);
        if (strcmp(resChars,"IntegerVector")==0)
	  graph->getLocalProperty<IntegerVectorProperty>(textString);
        if (strcmp(resChars,"SizeVector")==0)
	  graph->getLocalProperty<SizeVectorProperty>(textString);
        if (strcmp(resChars,"ColorVector")==0)
	  graph->getLocalProperty<ColorVectorProperty>(textString);
        setGraph(graph);
        emit newPropertySignal(graph, textString);
      }
    }
  }
  //=================================================
  void PropertyDialog::toStringProperty() {
    if (editedProperty==0) return;
    string name=editedPropertyName;
    if (name == "viewLabel") return;
    Observable::holdObservers();
    // allow to undo
    graph->push();
    PropertyInterface *newLabels=graph->getProperty(name);
    StringProperty *labels=graph->getLocalProperty<StringProperty>("viewLabel");
    if (tabWidget->currentIndex()==0) {
      Iterator<node> *itN;
      if (_filterSelection)
	itN = graph->getProperty<BooleanProperty>("viewSelection")->
	  getNonDefaultValuatedNodes();
      else {
	labels->setAllNodeValue( newLabels->getNodeDefaultStringValue() );
	itN = graph->getNodes();
      }
      while(itN->hasNext()) {
	node itn=itN->next();
	labels->setNodeValue(itn,  newLabels->getNodeStringValue(itn) );
      } delete itN;
    }
    else {
      Iterator<edge> *itE;
      if (_filterSelection)
	itE = graph->getProperty<BooleanProperty>("viewSelection")->
	  getNonDefaultValuatedEdges();
      else {
	labels->setAllEdgeValue( newLabels->getEdgeDefaultStringValue() );
	itE = graph->getEdges();
      }
      while(itE->hasNext()) {
	edge ite=itE->next();
	labels->setEdgeValue(ite,newLabels->getEdgeStringValue(ite));
      } delete itE;
    }
    /*glWidget->getScene()->getGlGraphComposite()->getInputData()->reloadLabelProperty();
      glWidget->draw();*/
    Observable::unholdObservers();
  }
  //=================================================
  void PropertyDialog::removeProperty() {
    if (editedProperty==0) return;
    if(graph->existLocalProperty(editedPropertyName)) {
      // allow to undo
      graph->push();
      graph->delLocalProperty(editedPropertyName);
      //setGlMainWidget(glWidget);
      setGraph(graph);
      editedProperty=0;
      emit removePropertySignal(graph,editedPropertyName);
    }
    else
      QMessageBox::critical( 0, "Tulip Property Editor Remove Failed",
			     QString("You cannot remove an inherited property,\n")
			     );
  }
  //=================================================
  void PropertyDialog::cloneProperty() {
    if (!graph || !editedProperty) return;
    // save editedProperty in local variable
    // to avoid any reset due to a setGraph call
    // from a addLocalProperty observer
    PropertyInterface *editedProp = editedProperty;
    CopyPropertyDialog dialog(parentWidget());
    vector<string> localProps;
    vector<string> inheritedProps;
    PropertyInterface* prop;
    Graph *parent = graph->getSuperGraph();
    if (parent == graph)
      parent = 0;
    forEach(prop, graph->getLocalObjectProperties()) {
      if (typeid(*prop) == typeid(*editedProp)) {
	const string &pName = prop->getName();
        if (pName != editedPropertyName)
          localProps.push_back(pName);
        if (parent && parent->existProperty(pName))
          inheritedProps.push_back(pName);
      }
    }
    forEach(prop, graph->getInheritedObjectProperties()) {
      const string &pName = prop->getName();
      if ((pName != editedPropertyName) &&
          (typeid(*prop) == typeid(*editedProp)))
        inheritedProps.push_back(pName);
    }
    dialog.setProperties(editedPropertyName, localProps, inheritedProps);
    CopyPropertyDialog::destType type;
    std::string text;
    if (dialog.getDestinationProperty(type, text)) {
      if (text.size() > 0) {
	if (type != CopyPropertyDialog::INHERITED) {
	  if (graph->existProperty(text)) {
	    if (typeid(*graph->getProperty(text))!=typeid(*editedProp)) {
	      QMessageBox::critical(parentWidget(), "Tulip Warning" ,"Properties are not of the same type.");
	      return;
	    }
	    if (type == CopyPropertyDialog::NEW &&
		graph->existLocalProperty(text)) {
	      if (text == editedPropertyName) {
		QMessageBox::critical(parentWidget(), "Tulip Warning",
				      "Properties are the same.");
		return;
	      } else if (QMessageBox::question(parentWidget(),
					       "Copy confirmation",
					       (std::string("Property ") + text + " already exists,\ndo you really want to overwrite it ?").c_str(),
					       QMessageBox::Ok,
					       QMessageBox::Cancel)
			 == QDialog::Rejected)
		return;
	    }
	  }
	  Observable::holdObservers();
	  // allow to undo
	  graph->push();
	  if (typeid((*editedProp)) == typeid(DoubleProperty))
	    {*graph->getLocalProperty<DoubleProperty>(text)=*((DoubleProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(LayoutProperty))
	    {*graph->getLocalProperty<LayoutProperty>(text)=*((LayoutProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(StringProperty))
	    {*graph->getLocalProperty<StringProperty>(text)=*((StringProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(BooleanProperty))
	    {*graph->getLocalProperty<BooleanProperty>(text)=*((BooleanProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(IntegerProperty))
	    {*graph->getLocalProperty<IntegerProperty>(text)=*((IntegerProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(ColorProperty))
	    {*graph->getLocalProperty<ColorProperty>(text)=*((ColorProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(SizeProperty))
	    {*graph->getLocalProperty<SizeProperty>(text)=*((SizeProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(DoubleVectorProperty))
	    {*graph->getLocalProperty<DoubleVectorProperty>(text)=*((DoubleVectorProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(CoordVectorProperty))
	    {*graph->getLocalProperty<CoordVectorProperty>(text)=*((CoordVectorProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(StringVectorProperty))
	    {*graph->getLocalProperty<StringVectorProperty>(text)=*((StringVectorProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(BooleanVectorProperty))
	    {*graph->getLocalProperty<BooleanVectorProperty>(text)=*((BooleanVectorProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(IntegerVectorProperty))
	    {*graph->getLocalProperty<IntegerVectorProperty>(text)=*((IntegerVectorProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(ColorVectorProperty))
	    {*graph->getLocalProperty<ColorVectorProperty>(text)=*((ColorVectorProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(SizeVectorProperty))
	    {*graph->getLocalProperty<SizeVectorProperty>(text)=*((SizeVectorProperty*)editedProp);}
	  //}
	} else {
	  Graph *parent = graph->getSuperGraph();
	  Observable::holdObservers();
	  // allow to undo
	  parent->push();
	  if (typeid((*editedProp)) == typeid(DoubleProperty))
	    {*parent->getProperty<DoubleProperty>(text)=*((DoubleProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(LayoutProperty))
	    {*parent->getProperty<LayoutProperty>(text)=*((LayoutProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(StringProperty))
	    {*parent->getProperty<StringProperty>(text)=*((StringProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(BooleanProperty))
	    {*parent->getProperty<BooleanProperty>(text)=*((BooleanProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(IntegerProperty))
	    {*parent->getProperty<IntegerProperty>(text)=*((IntegerProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(ColorProperty))
	    {*parent->getProperty<ColorProperty>(text)=*((ColorProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(SizeProperty))
	    {*parent->getProperty<SizeProperty>(text)=*((SizeProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(DoubleVectorProperty))
	    {*parent->getProperty<DoubleVectorProperty>(text)=*((DoubleVectorProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(CoordVectorProperty))
	    {*parent->getProperty<CoordVectorProperty>(text)=*((CoordVectorProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(StringVectorProperty))
	    {*parent->getProperty<StringVectorProperty>(text)=*((StringVectorProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(BooleanVectorProperty))
	    {*parent->getProperty<BooleanVectorProperty>(text)=*((BooleanVectorProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(IntegerVectorProperty))
	    {*parent->getProperty<IntegerVectorProperty>(text)=*((IntegerVectorProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(ColorVectorProperty))
	    {*parent->getProperty<ColorVectorProperty>(text)=*((ColorVectorProperty*)editedProp);}
	  if (typeid((*editedProp)) == typeid(SizeVectorProperty))
	    {*parent->getProperty<SizeVectorProperty>(text)=*((SizeVectorProperty*)editedProp);}
	}
	setGraph(graph);
	Observable::unholdObservers();
      }else{
	QMessageBox::warning(this, tr("Fail to copy property"),
			     "You can't create a property with empty name",
			     QMessageBox::Ok,
			     QMessageBox::Ok);
      }
    }
  }
  //=================================================
  void PropertyDialog::importCSVData(){
     if(graph!=NULL){
     ImportCSVDataDialog dialog(new ImportCSVDataWidget(),graph,this);
     dialog.setWindowTitle(tr("Import CSV data"));
     graph->push();
     Observable::holdObservers();
     if(dialog.exec()==QDialog::Rejected){
       graph->pop(false);
     }
     Observable::unholdObservers();
     }
   }
}
