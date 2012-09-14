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

#include <tulip/CoordEditor.h>
#include <ui_CoordEditor.h>
#include <cfloat>
using namespace tlp;

CoordEditor::CoordEditor(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CoordEditor) {
  ui->setupUi(this);
  QDoubleValidator *validator = new QDoubleValidator(this);
  validator->setRange(-FLT_MAX,FLT_MAX,1000);
  ui->xLineEdit->setValidator(validator);
  ui->yLineEdit->setValidator(validator);
  ui->zLineEdit->setValidator(validator);
  setCoord(Coord());
  connect(ui->xLineEdit,SIGNAL(textChanged(QString)),this,SLOT(coordUpdated()));
  connect(ui->yLineEdit,SIGNAL(textChanged(QString)),this,SLOT(coordUpdated()));
  connect(ui->zLineEdit,SIGNAL(textChanged(QString)),this,SLOT(coordUpdated()));
}

CoordEditor::~CoordEditor() {
  delete ui;
}

Coord CoordEditor::coord()const {
  return Coord(ui->xLineEdit->text().toFloat(),ui->yLineEdit->text().toFloat(),ui->zLineEdit->text().toFloat());
}
void CoordEditor::setCoord(const Coord& coord) {
  blockSignals(true);
  ui->xLineEdit->setText(QString::number(coord[0]));
  ui->yLineEdit->setText(QString::number(coord[1]));
  ui->zLineEdit->setText(QString::number(coord[2]));
  blockSignals(false);
  coordUpdated();
}

void CoordEditor::coordUpdated() {
  emit(coordChanged(coord()));
}
