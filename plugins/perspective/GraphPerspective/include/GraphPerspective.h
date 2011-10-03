#ifndef GRAPHPERSPECTIVE_H
#define GRAPHPERSPECTIVE_H

#include <tulip/Perspective.h>

class GraphHierarchiesModel;
class GraphHierarchiesEditor;

namespace Ui {
class GraphPerspectiveMainWindowData;
}

class GraphPerspective : public tlp::Perspective {
  Q_OBJECT
  Ui::GraphPerspectiveMainWindowData *_ui;
  GraphHierarchiesModel *_graphs;
  bool _maximised;
public:
  GraphPerspective(tlp::PerspectiveContext &c);
  virtual void construct(tlp::PluginProgress *);

protected slots:
  void refreshDockExpandControls();
  void showFullScreen(bool);
};

#endif // GRAPHPERSPECTIVE_H
