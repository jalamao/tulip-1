//-*-c++-*-

#ifndef viewGl_included
#define viewGl_included
#include <map>

#if (__GNUC__ < 3)
#include <hash_map>
#else
#include <ext/hash_map>
#endif

#include <vector>
#include <qlistview.h>
#include <qtextview.h> 
#include <qsplitter.h>
#include <qpopupmenu.h>
#include <qstring.h>
#include <qmainwindow.h>
#include <pthread.h>
#include <string>
#include <tulip/Reflect.h>
#include <tulip/SuperGraph.h>
#include <tulip/Observable.h>
#include "TulipData.h"

class Cluster;
class QWorkspace;
class QProgressDialog;
class QEvent;
class PropertyDialog;
//class NavigateGlGraph;
class GlGraphWidget;
class GlGraph;
class View3DSetup;
class ClusterTree;
class TulipElementProperties;
class ToolBar;
class Overview;
class Morphing;

///Widget for manipulation and visualization of a graph
class viewGl : public TulipData, Observer {
  Q_OBJECT;

public:
  viewGl(QWidget *parent=NULL, const char *name=NULL);
  virtual ~viewGl();
  void setParameters(const DataSet);
  bool eventFilter(QObject *, QEvent *);

protected:
  ClusterTree *clusterTreeWidget;
  ToolBar *mouseToolBar;
  Overview *overviewWidget;
  QWidget *aboutWidget;
  QDockWindow *overviewDock;
  QDockWindow *tabWidgetDock;
  QDockWindow *mouseToolBarDock;
  GlGraphWidget *glWidget;
  PropertyDialog *propertiesWidget;
  TulipElementProperties *nodeProperties;
  SuperGraph * copyCutPasteGraph;
  bool elementsDisabled;

  //  QPopupMenu propertyMenu;
  QPopupMenu layoutMenu;
  QPopupMenu metricMenu;
  QPopupMenu colorsMenu;
  QPopupMenu sizesMenu;
  QPopupMenu intMenu;
  QPopupMenu stringMenu;
  QPopupMenu importGraphMenu;
  QPopupMenu exportGraphMenu;
  QPopupMenu graphMenu;
  //  QPopupMenu clusteringMenu;
  QPopupMenu optionMenu;
  //QPopupMenu viewMenu;
  QPopupMenu selectMenu;
  //QPopupMenu editMenu;
  QPopupMenu exportImageMenu;
  QPopupMenu dialogMenu;

  //QPopupMenu* windowsMenu;
  void focusInEvent ( QFocusEvent * );
  typedef std::set< Observable * >::iterator ObserverIterator;
  void update ( ObserverIterator begin, ObserverIterator end);
  void initObservers();
  void clearObservers();
  void enableQPopupMenu(QPopupMenu *, bool);
  void enableElements(bool);
  void setNavigateCaption(std::string);
  void initializeGraph(SuperGraph *);
  void initializeGlGraph(GlGraph *);

public slots:
  void startTulip();
  void fileOpen(std::string *,QString &);
  void closeEvent(QCloseEvent *e); 
  
protected slots:
  void helpAbout();
  void fileExit();
  void fileSave();
  void fileSaveAs();
  int  closeWin();
  void windowActivated(QWidget *);
  void hierarchyChangeSuperGraph(SuperGraph *);
  void fileNew();
  void fileOpen();  
  void filePrint();
  void editCut();
  void editCopy();
  void editPaste();
  void editFind();
  void changeMetric(int);
  void changeString(int);
  void changeSizes(int);
  void changeColors(int);
  void changeInt(int);
  void changeSelection(int);
  void changeLayout(int);
  void importGraph(int);
  void exportGraph(int);
  void exportImage(int);
  void makeClustering(int );
  void outputEPS();
  void showDialog(int);
  void redrawView();
  void centerView();
  void updateSatutBar();
  void deselectALL();
  void reverseSelection();
  void delSelection();
  void newSubgraph();
  void reverseSelectedEdgeDirection();
  void windowsMenuAboutToShow();
  void windowsMenuActivated( int id );
  void restoreView();
  void new3DView();
  void goInside();
  void changeSuperGraph(SuperGraph *);
  void superGraphAboutToBeRemoved(SuperGraph *);
  void glGraphWidgetClosed(GlGraphWidget *);
  void ungroup();  
  void group();  

private:
  template<typename PROPERTY> bool changeProperty(std::string, std::string, bool = true, bool = false );
  GlGraphWidget *newOpenGlView(SuperGraph *graph,const QString &);
  stdext::hash_map<unsigned int, std::string> openFiles;
  void buildMenus();
  bool fileSave(std::string plugin, std::string filename);
  int alreadyTreated(std::set<unsigned int>, SuperGraph *);
  unsigned int mouseClicX,mouseClicY;
  Morphing *morph;
};

#endif // viewGl_included

