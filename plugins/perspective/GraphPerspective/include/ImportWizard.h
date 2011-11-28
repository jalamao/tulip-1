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
#ifndef IMPORTWIZARD_H
#define IMPORTWIZARD_H

#include <QtGui/QWizard>
#include <tulip/WithParameter.h>

namespace Ui {
class ImportWizard;
}
class PanelSelectionWizard;

namespace tlp {
class GraphHierarchiesModel;
}

class ImportWizard: public QWizard {
  Q_OBJECT

  Ui::ImportWizard* _ui;
  PanelSelectionWizard* _panelWizard;
public:
  explicit ImportWizard(tlp::GraphHierarchiesModel*model, QWidget *parent = 0);
  virtual ~ImportWizard();

  QString algorithm() const;
  QString group() const;
  tlp::DataSet parameters() const;

  bool createPanel() const;
  QString panelName() const;

protected slots:
  void groupSelected(const QString&);
  void algorithmSelected(const QString&);
  void updateFinishButton();
  void panelDoubleClicked();
};

#endif // IMPORTWIZARD_H
