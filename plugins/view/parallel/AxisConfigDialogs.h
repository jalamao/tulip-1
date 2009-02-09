//-*-c++-*-
/*
 Author: Antoine Lambert

 Email : antoine.lambert@labri.fr

 Last modification : 12/08

 This program is free software; you can redistribute it and/or modify  *
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
*/

#ifndef AXISCONFIGDIALOGS_H_
#define AXISCONFIGDIALOGS_H_

#ifndef DOXYGEN_NOTFOR_DEVEL

#include <QtGui/QDialog>

#include <QtGui/QSpinBox>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QCloseEvent>
#include <QtGui/QComboBox>
#include <QtGui/QCheckBox>

#include <tulip/ItemsListWidget.h>

#include "QuantitativeParallelAxis.h"
#include "NominalParallelAxis.h"

namespace tlp {

// A really simple dialog which allows user to set number of graduations
// and data ordering (ascendent or not) for a particular quantitative axis
class QuantitativeAxisConfigDialog : public QDialog {

	Q_OBJECT

public :

	QuantitativeAxisConfigDialog(QuantitativeParallelAxis *axis);
	void closeEvent (QCloseEvent * event);

private :

	void initAxisPartitionsTable();

	QuantitativeParallelAxis *axis;
	QSpinBox *nbGrads;
	QComboBox *axisOrder;
	QPushButton *okButton;
	QCheckBox *log10Scale;
};


// A really simple dialog which allows user to set labels order
// on a particular nominative axis
class NominalAxisConfigDialog : public QDialog {

	Q_OBJECT

public :

	NominalAxisConfigDialog(NominalParallelAxis *axis);
	void closeEvent (QCloseEvent * event);

public slots :

	void pressButtonUp();
	void pressButtonDown();
	void pressButtonLexOrder();

private :

	NominalParallelAxis *axis;
	QPushButton *okButton;
	QHash<QString,itemInfo > hashDataBase;
	ItemsListWidget *axisLabelsOrder;
};

}

#endif // DOXYGEN_NOTFOR_DEVEL

#endif /* AXISCONFIGDIALOGS_H_ */

