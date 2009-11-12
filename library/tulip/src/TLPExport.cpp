#include <iostream>
#include <time.h>
#include <string.h>

#include <tulip/TulipPlugin.h>
#include <tulip/Coord.h>
#include <tulip/AbstractProperty.h>

#ifndef DOXYGEN_NOTFOR_DEVEL

#define TLP_FILE_VERSION "2.0"

using namespace std;
using namespace tlp;

static string convert(const string & tmp) {
  string newStr;
  for (unsigned int i=0; i<tmp.length(); ++i) {
    if (tmp[i]=='\"')
      newStr+="\\\"";
    else
      if (tmp[i]=='\n')
	newStr+="\\n";
      else
	if (tmp[i] == '\\')
	  newStr+="\\\\";
	else
	  newStr+=tmp[i];
  }
  return newStr;
}

static const char* boolTN, *colorTN, *coordTN, *doubleTN, *floatTN, *intTN, *stringTN, *uintTN, *DataSetTN;
static bool typesInited = false;

namespace {
  static const char * paramHelp[] = {
    // name
    HTML_HELP_OPEN() \
    HTML_HELP_DEF( "type", "string" ) \
    HTML_HELP_DEF( "default", "" ) \
    HTML_HELP_BODY() \
    "Indicates the name of this graph." \
    HTML_HELP_CLOSE(),
    // author
    HTML_HELP_OPEN() \
    HTML_HELP_DEF( "type", "string" ) \
    HTML_HELP_DEF( "default", "" ) \
    HTML_HELP_BODY() \
    "Indicates the author of this graph." \
    HTML_HELP_CLOSE(),
    // comments
    HTML_HELP_OPEN() \
    HTML_HELP_DEF( "type", "string" ) \
    HTML_HELP_DEF( "default", "This file was generated by Tulip." ) \
    HTML_HELP_BODY() \
    "adds some comments." \
    HTML_HELP_CLOSE(),
  };
}

// workaround for G++ bug for <<
inline void printGraph(std::ostream &os, tlp::Graph *graph) {
  os << graph << endl;
}

namespace tlp {
#endif //DOXYGEN_NOTFOR_DEVEL

/** \addtogroup export */
/*@{*/
/// Export plugin for TLP format.
/**
 * This plugin records a Tulip graph structure in a file using the TLP format.
 * TLP is the Tulip Software Graph Format.
 * See 'Tulip-Software.org->Docs->TLP File Format' for description.
 * Note: When using the Tulip graphical user interface,
 * choosing "File->Export->TLP" menu item is the same that using
 * "File->Save as" menu item.
 */
class TLPExport:public ExportModule {
public:
  DataSet controller;

  TLPExport(AlgorithmContext context):ExportModule(context) {
    addParameter<string>("name", paramHelp[0]);
    addParameter<string>("author", paramHelp[1]);
    addParameter<string>("text::comments", paramHelp[2], "This file was generated by Tulip.");
    addParameter<DataSet>("controller");
  }
    //=====================================================
  ~TLPExport(){}
    //=====================================================
  void saveGraphElements(ostream &os, Graph *graph) {
    if (graph->getSuperGraph() != graph) {
      os << "(cluster " << graph->getId() << " \"" << graph->getAttribute<string>("name") << "\"" << endl;
      Iterator<node> *itN = graph->getNodes();
      if (itN->hasNext()) {
	os << "(nodes ";
	while (itN->hasNext()) {
	  os << itN->next().id;
	  if (itN->hasNext()) os << " ";
	}
	os << ")" << endl;
      } delete itN;
      Iterator<edge> *itE = graph->getEdges();
      if (itE->hasNext()) {
	os << "(edges ";
	while (itE->hasNext()) {
	  os << itE->next().id;
	  if (itE->hasNext()) os << " ";
	}
	os << ")" << endl;
      } delete itE;
    }
    else
      printGraph(os, graph);
    Iterator<Graph *> *itS = graph->getSubGraphs();
    while (itS->hasNext())
      saveGraphElements(os,itS->next());
    delete itS;
    if (graph->getSuperGraph() != graph)  os << ")" << endl;
  }
  //=====================================================
  void saveLocalProperties(ostream &os,Graph *graph) {
    Iterator<PropertyInterface *> *itP=graph->getLocalObjectProperties();
    PropertyInterface *prop;
    while (itP->hasNext()) {
      prop = itP->next();
      if (graph->getSuperGraph()==graph)
	os << "(property " << " 0 " << prop->getTypename() << " " ;
      else
	os << "(property " << " " << graph->getId() << " " << prop->getTypename() << " " ;
      os << "\"" << convert(prop->getName()) << "\"" << endl;
      string nDefault = prop->getNodeDefaultStringValue();
      string eDefault = prop->getEdgeDefaultStringValue();
      os <<"(default \"" << convert(nDefault) << "\" \"" << convert(eDefault) << "\")" << endl;
      Iterator<node> *itN=graph->getNodes();
      while (itN->hasNext()) {
	node itn=itN->next();
	string tmp = prop->getNodeStringValue(itn);
	if (strcmp(tmp.c_str(),nDefault.c_str())!=0) os << "(node " << itn.id << " \"" << convert(tmp) << "\")" << endl ;
      } delete itN;
      Iterator<edge> *itE=graph->getEdges();
      while (itE->hasNext()) {
	edge ite=itE->next();
	string tmp = prop->getEdgeStringValue(ite);
	if (strcmp(tmp.c_str(),eDefault.c_str())!=0) os << "(edge " << ite.id << " \"" << convert(tmp) << "\")" << endl ;
      } delete itE;
      os << ")" << endl;
    }
    delete itP;
  }
  //=====================================================
  void saveProperties(ostream &os,Graph *graph) {
    saveLocalProperties(os,graph);
    Iterator<Graph *> *itS=graph->getSubGraphs();
    while (itS->hasNext())
      saveProperties(os,itS->next());
    delete itS;
  }
  //=====================================================
  void initTypeNames() {
    if (!typesInited) {
      boolTN = typeid(typesInited).name();
      Color color;
      colorTN = typeid(color).name();
      Coord coord;
      coordTN = typeid(coord).name();
      double d;
      doubleTN = typeid(d).name();
      float f;
      floatTN = typeid(f).name();
      int i;
      intTN = typeid(i).name();
      string s;
      stringTN = typeid(s).name();
      unsigned int ui;
      uintTN = typeid(ui).name();
      DataSet dataSet;
      DataSetTN = typeid(dataSet).name();
      typesInited = true;
    }
  }
  //=====================================================
  void saveDataSet(ostream &os, const DataSet &data) {
    initTypeNames();
    // get iterator over pair attribute/value
    Iterator< pair<string, DataType*> > *it = data.getValues();
    while( it->hasNext() ) {
      pair<string, DataType*> p;
      p = it->next();
      const string tn = p.second->typeName;
       // get output type name
      if (tn == boolTN)
	os << '(' << "bool";
      else if (tn == colorTN)
	os << '(' << "color";
      else if (tn == coordTN)
	os << '(' << "coord";
      else if (tn == doubleTN)
	os << '(' << "double";
      else if (tn == floatTN)
	os << '(' << "float";
      else if (tn == intTN)
	os << '(' << "int";
      else if (tn == stringTN)
	os << '(' << "string";
      else if (tn == uintTN)
	os << '(' << "uint";
      else if (tn == DataSetTN)
	os << '(' << "DataSet";
      // general case do nothing
      else continue;
      // output attribute name
      os << " \"" << p.first << "\" ";
      // output value
      // special cases
      if (tn == boolTN)
	os << ((*(bool*)p.second->value) ? "true" : "false");
      else if (tn == colorTN) {
	Color *color = (Color*) p.second->value;
	//os << "\"(" << (int)color->getR() << ',' << (int)color->getG() << ',' << (int) color->getB() << ",0)\"";
	os << "\"" << convert(ColorType::toString(*color)) << "\"";
      } else if (tn == coordTN) {
	Coord *coord =  (Coord*) p.second->value;
	//os << "\"(" << coord->getX() << "," << coord->getY() << "," << coord->getZ() << ")\"";
	os << "\"" << convert(PointType::toString(*coord)) << "\"";
      }	else if (tn == DataSetTN) {
	os << endl;
	DataSet *dataSet = (DataSet*) p.second->value;
	saveDataSet(os, *dataSet);
      }else if (tn == doubleTN)
	os << *(double*)p.second->value;
      else if (tn == floatTN)
	os << *(float*)p.second->value;
      else if (tn == intTN)
	os << *(int*)p.second->value;
      else if (tn == stringTN)
	os << '"' << *(string *)p.second->value << '"';
      else if (tn == uintTN)
	os << *(unsigned int *) p.second->value;
      // output closed parenthesis
      os << ')' << endl;
    } delete it;
  }
  //=====================================================
  void saveAttributes(ostream &os, Graph *graph) {
    os << "(attributes " << endl;
    saveDataSet(os, graph->getAttributes());
    os << ")" << endl;
  }
  //=====================================================
  void saveController(ostream &os, DataSet &data) {
    os << "(controller " << endl;
    saveDataSet(os, data);
    os << ")" << endl;
  }

  bool exportGraph(ostream &os,Graph *currentGraph) {
    graph=currentGraph->getRoot();
    string name;
    string author;
    string comments;

    if (dataSet != NULL) {
      dataSet->get("name", name);
      dataSet->get("author", author);
      dataSet->get("text::comments", comments);
    }

    if (name.length() > 0)
      graph->setAttribute("name", name);

    // get ostime
    time_t ostime = time(NULL);
    // get local time
    struct tm *currTime = localtime(&ostime);
    // format date
    char currDate[32];
    strftime(currDate, 32, "%m-%d-%Y", currTime);

    // output tlp format version
    os << "(tlp \"" << TLP_FILE_VERSION << '"' << endl;
    // current date
    os << "(date \"" << currDate << "\")" << endl;
    // author
    if (author.length() > 0)
      os << "(author \"" << author << "\")" << endl;
    // comments
    os << "(comments \"" << comments << "\")" << endl;

    saveGraphElements(os,graph);
    saveProperties(os,graph);
    saveAttributes(os,graph);

    //Save displaying
    //    DataSet displaying;
    /*if (dataSet != NULL && dataSet->get<DataSet>("displaying", displaying))
      saveDisplaying(os, displaying);*/

    //Save views
     if (dataSet != NULL && dataSet->get<DataSet>("controller", controller))
      saveController(os, controller);

    os << ')' << endl; // end of (tlp ...
    return true;
  }
};
  EXPORTPLUGIN(TLPExport,"tlp","Auber David","31/07/2001","TLP Export plugin", "1.1");
/*@}*/
}
