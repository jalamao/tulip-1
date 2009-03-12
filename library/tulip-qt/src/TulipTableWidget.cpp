#include <iostream>
#include <sstream>
#include <string>

#include <QtCore/qstring.h>
#include <QtGui/qfiledialog.h>
#include <QtGui/qcolordialog.h>
#include <QtGui/qpainter.h>
#include <QtGui/qfontmetrics.h>
#include <QtGui/qapplication.h>
#include <QtGui/qlayout.h>
#include <QtGui/qlineedit.h>
#include <QtCore/qdir.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qmetaobject.h>
#include <QtGui/qvalidator.h>
#include <QtGui/qsizepolicy.h>
#include <QtGui/qmessagebox.h>
#include <QtGui/qinputdialog.h>
#include <QtGui/qevent.h>
#include <QtGui/qcheckbox.h>
#include <QtGui/qitemeditorfactory.h>

#include <tulip/Graph.h>
#include <tulip/DoubleProperty.h>
#include <tulip/StringProperty.h>
#include <tulip/BooleanProperty.h>
#include <tulip/LayoutProperty.h>
#include <tulip/IntegerProperty.h>
#include <tulip/ColorProperty.h>
#include <tulip/SizeProperty.h>
#include <tulip/GlyphManager.h>
#include <tulip/GlGraphStaticData.h>
#include <tulip/Glyph.h>

#include "tulip/TulipTableWidget.h"

#define COLORTABLEITEM_RTTI 1001
#define FILETABLEITEM_RTTI  1002
#define SIZETABLEITEM_RTTI  1003
#define COORDTABLEITEM_RTTI 1004
#define GLYPHTABLEITEM_RTTI 1005
#define SELECTIONTABLEITEM_RTTI 1006
#define EDGESHAPETABLEITEM_RTTI 1007
#define LABELPOSITIONTABLEITEM_RTTI 1008

#define ROW_HEIGHT 18

using namespace std;
using namespace tlp;
//================================================================================
ColorButton::ColorButton(const QRgb &c, QWidget *parent, const char *name, Qt::WFlags f) :
    QPushButton(parent), color(c) {
  connect(this, SIGNAL(pressed()), SLOT(colorDialog()));
  emit pressed();
  setFocusPolicy(Qt::StrongFocus);
}
ColorButton::~ColorButton() {}
QRgb ColorButton::getColor() const {
  return color;
}
void ColorButton::colorDialog() {
  bool ok;
  QRgb tmpcolor = QColorDialog::getRgba(color, &ok);
  if (ok) color = tmpcolor;
}
void ColorButton::paintEvent(QPaintEvent *qpe) {
  QPainter p(this);
  p.fillRect(qpe->rect(), QColor(color));
}
//================================================ 
class ColorTableItem : public TulipTableWidgetItem {
private:
  QRgb color;
public:
  ColorTableItem(const QRgb &color);
  ~ColorTableItem();
  QRgb getColor() const;
  
  QWidget *createEditor(QTableWidget *table) const;
  void setContentFromEditor(QWidget *w);
};

ColorTableItem::ColorTableItem(const QRgb &color) :
  TulipTableWidgetItem(COLORTABLEITEM_RTTI), color(color) {
  setText(ColorType::toString(Color(qRed(color), qGreen(color),
				    qBlue(color), qAlpha(color))).c_str());
}
ColorTableItem::~ColorTableItem() {}
QRgb ColorTableItem::getColor() const {return color;}
QWidget *ColorTableItem::createEditor(QTableWidget* table) const {
  ColorButton *cb = new ColorButton(color, table->viewport());
  return cb;
}
void ColorTableItem::setContentFromEditor(QWidget *w) {
  //  cerr << __PRETTY_FUNCTION__ << endl;
  color = ((ColorButton *)w)->getColor();
  setText(ColorType::toString(Color(qRed(color), qGreen(color),
				    qBlue(color), qAlpha(color))).c_str());
}
//================================================================================
FilenameEditor::FilenameEditor(QWidget *parent) : QWidget(parent) {
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0);
  lineedit = new QLineEdit(this);
  lineedit->setFrame(false);
  lineedit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  connect(lineedit, SIGNAL(textChanged(const QString &)),
          this, SIGNAL(fileNameChanged(const QString &)));
  layout->addWidget(lineedit);
  button = new QPushButton("...", this);
  button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding));
  button->setFixedWidth(button->fontMetrics().width(" ... "));
  layout->addWidget(button);
  connect(button, SIGNAL(clicked()), this, SLOT(buttonPressed()));
  setFocusProxy(lineedit);
}
FilenameEditor::~FilenameEditor() {}
QString FilenameEditor::fileName() const {
  return lineedit->text();
}
void FilenameEditor::setFileName(const QString &s) {
  //cerr << __PRETTY_FUNCTION__ << (const char *) s << endl;
  lineedit->setText(s);
}
QString FilenameEditor::filter() const {
  return fileFilter;
}
void FilenameEditor::setFilter(const QString &f) {
  fileFilter = f;
}
void FilenameEditor::buttonPressed() {
  QFileDialog *dlg = new QFileDialog(this,
				     "Choose a file",
				     "./",
                                     fileFilter);
  dlg->setModal(true);
  dlg->setFileMode(QFileDialog::ExistingFile);
  if (dlg->exec() == QDialog::Accepted ) {
    QString file = dlg->selectedFiles().first();
    if (!file.isNull()) {
      //QStringList currentDir = QStringList::split(QDir::separator(), QDir::currentDirPath(), true);
      //QStringList filePath = QStringList::split(QDir::separator(), QFileInfo(file).dirPath(true), true);
      QStringList currentDir = QDir::currentPath().split(QDir::separator());
      QStringList filePath = QFileInfo(file).dir().absolutePath().split(QDir::separator());
     QString relativePath = "";
      while((!currentDir.empty() && !filePath.empty())
            && (currentDir.front() == filePath.front())) {
	currentDir.pop_front();
	filePath.pop_front();
      }
      while (!currentDir.empty()) {
        relativePath += "..";
        relativePath += QDir::separator();
        currentDir.pop_front();
      }
      if (!filePath.empty())
        relativePath += filePath.join((const QString) (QChar) QDir::separator()) + QDir::separator();
      setFileName(relativePath + QFileInfo(file).fileName());
    }
  }
  delete dlg;
  // button->setDown(false);
}
//================================================================================
class FileTableItem : public TulipTableWidgetItem {
public:
  FileTableItem(QString s);
  ~FileTableItem();
  QWidget *createEditor(QTableWidget* table) const;
  void setContentFromEditor(QWidget *w);
};

FileTableItem::FileTableItem(QString s) : TulipTableWidgetItem(s, FILETABLEITEM_RTTI) {}
FileTableItem::~FileTableItem() {}
QWidget *FileTableItem::createEditor(QTableWidget* table) const {
  FilenameEditor *w = new FilenameEditor(table->viewport());
  w->setFilter("Images (*.png *.jpeg *.jpg *.bmp)");
  w->setFileName(text());
  return w;
}
void FileTableItem::setContentFromEditor(QWidget *w) {
  //cerr << __PRETTY_FUNCTION__ << endl;
  QString s = ((FilenameEditor *)w)->fileName();
  //cerr << "String : " << (const char *) s << endl;
  if (!s.isNull()) {
    setText(s);
    qApp->processEvents();
  }
}
//================================================================================
SizeEditor::SizeEditor(const Size &s, QWidget *parent) : QWidget(parent), size(s) {
  setAutoFillBackground(true);
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setSpacing(3);
  layout->setMargin(0);
  QDoubleValidator *validator = new QDoubleValidator(this);
  stringstream ss;
  ss << size.getW() << " " << size.getH() << " " << size.getD();
  for (int i=0; i<3; ++i) {
    string str;
    ss >> str;
    edit[i] = new QLineEdit(str.c_str(), this);
    edit[i]->setValidator(validator);
    edit[i]->setFrame(true);
    edit[i]->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    layout->addWidget(edit[i]);
  }
  connect(edit[0], SIGNAL(textChanged(const QString &)), this, SLOT(changeW(const QString &)));
  connect(edit[1], SIGNAL(textChanged(const QString &)), this, SLOT(changeH(const QString &)));
  connect(edit[2], SIGNAL(textChanged(const QString &)), this, SLOT(changeD(const QString &)));
  setFocusPolicy(Qt::StrongFocus);
}

SizeEditor::~SizeEditor() {}
Size SizeEditor::getSize() const {return size;}
void SizeEditor::setFocus() {edit[0]->setFocus();}
void SizeEditor::changeW(const QString &s) {
  stringstream ss; ss << s.toAscii().data();
  float f; ss >> f;
  size.setW(f);
}
void SizeEditor::changeH(const QString &s) {
  stringstream ss; ss << s.toAscii().data();
  float f; ss >> f;
  size.setH(f);
}
void SizeEditor::changeD(const QString &s) {
  stringstream ss; ss << s.toAscii().data();
  float f; ss >> f;
  size.setD(f);
}
//================================================================================
class SizeTableItem : public TulipTableWidgetItem {
private:
  Size size;
public:
  SizeTableItem(Size& s);
  ~SizeTableItem();
  
  void setSize(const Size &s);
  QWidget *createEditor(QWidget* p) const;
  void setContentFromEditor(QWidget *w);
};

SizeTableItem::SizeTableItem(Size& s) : TulipTableWidgetItem(SIZETABLEITEM_RTTI) {
  setSize(s);
}
SizeTableItem::~SizeTableItem() {}
void SizeTableItem::setSize(const Size &s) {
  size = s;
  setText(SizeType::toString(s).c_str());
}
QWidget *SizeTableItem::createEditor(QWidget* parent) const {
  SizeEditor *w = new SizeEditor(size, parent);
  return w;
}
void SizeTableItem::setContentFromEditor(QWidget *editor) {
  setSize(((SizeEditor *)editor)->getSize());
}
//================================================================================
CoordEditor::CoordEditor(const Coord &c, QWidget *parent) : QWidget(parent), coord(c)
{
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setSpacing(3);
  layout->setMargin(0);
  QDoubleValidator *validator = new QDoubleValidator(this);
  stringstream ss;
  ss << coord.getX() << " " << coord.getY() << " " << coord.getZ();
  for (int i=0; i<3; ++i) {
    string str;
    ss >> str;
    edit[i] = new QLineEdit(str.c_str(), this);
    edit[i]->setValidator(validator);
    edit[i]->setFrame(true);
    edit[i]->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    layout->addWidget(edit[i]);
  }
  connect(edit[0], SIGNAL(textChanged(const QString &)), this, SLOT(changeX(const QString &)));
  connect(edit[1], SIGNAL(textChanged(const QString &)), this, SLOT(changeY(const QString &)));
  connect(edit[2], SIGNAL(textChanged(const QString &)), this, SLOT(changeZ(const QString &)));
  setFocusPolicy(Qt::StrongFocus);
}

CoordEditor::~CoordEditor() {}

Coord CoordEditor::getCoord() const {return coord;}
void CoordEditor::setFocus() {edit[0]->setFocus();}

void CoordEditor::changeX(const QString &s) {
  stringstream ss; ss << s.toAscii().data();
  float f; ss >> f;
  coord.setX(f);
}
void CoordEditor::changeY(const QString &s) {
  stringstream ss; ss << s.toAscii().data();
  float f; ss >> f;
  coord.setY(f);
}
void CoordEditor::changeZ(const QString &s) {
  stringstream ss; ss << s.toAscii().data();
  float f; ss >> f;
  coord.setZ(f);
}
//================================================================================
class CoordTableItem : public TulipTableWidgetItem {
private:
  Coord coord;
public:
  CoordTableItem(Coord& c);
  ~CoordTableItem();
  
  void setCoord(const Coord &c);
  QWidget *createEditor(QTableWidget *table) const;
  void setContentFromEditor(QWidget *w);
};

CoordTableItem::CoordTableItem(Coord& c) : TulipTableWidgetItem(COORDTABLEITEM_RTTI) {
  setCoord(c);
}
CoordTableItem::~CoordTableItem() {}
  
void CoordTableItem::setCoord(const Coord &c) {
  coord = c;
  setText(PointType::toString(c).c_str());
} 
QWidget *CoordTableItem::createEditor(QTableWidget* table) const {
  CoordEditor *w = new CoordEditor(coord, table->viewport());
  return w;
}
void CoordTableItem::setContentFromEditor(QWidget *editor) {
  setCoord(((CoordEditor *)editor)->getCoord());
}
//================================================================================
class IntFromListEditor : public QComboBox {
  QStringList* values;
public:
  IntFromListEditor(QStringList *list, QWidget *widget = 0);
  ~IntFromListEditor() {}
private:
  void populate();
};

IntFromListEditor::IntFromListEditor(QStringList *list, QWidget *widget) :
  QComboBox(widget), values(list) {
  populate();
}

void IntFromListEditor::populate() {
  for (int i = 0; i < values->size(); ++i) {
    insertItem(i, values->at(i));
  }
}

class IntFromListTableItem : public TulipTableWidgetItem {
  QStringList* names;
  int value;
public:
  IntFromListTableItem(QStringList* l, int type):
    TulipTableWidgetItem(type), names(l) {}
  ~IntFromListTableItem() {}
  virtual QString valueToText(int) const {
    cerr << __PRETTY_FUNCTION__ << endl;
    return QString("error");
  }
  virtual int textToValue(const QString &s) const {
    cerr << __PRETTY_FUNCTION__ << endl;
    return 0;
  }

  void setValue(int val);

  QString textForTulip() const {
    return QString(IntegerType::toString(value).c_str());
  }

  QWidget *createEditor(QTableWidget* table) const;
  void setContentFromEditor(QWidget *w);
};

void IntFromListTableItem::setValue(int val) {
    value = val;
    setText(this->valueToText(val));
}

QWidget *IntFromListTableItem::createEditor(QTableWidget* table) const {
  IntFromListEditor *w = new IntFromListEditor(names,
					       table->viewport());
  QString name = valueToText(value);
  for(int i = 0; i < names->size(); ++i) {
    if (names->at(i) == name) {
      w->setCurrentIndex(i);
    }
  }
  return w;
}

void IntFromListTableItem::setContentFromEditor(QWidget *editor) {
  setValue(textToValue(names->at(((QComboBox *)editor)->currentIndex())));
}

//================================================================================
class GlyphTableItem : public IntFromListTableItem {
  static QStringList* glyphNames;
  QStringList* getGlyphNames() {
    if (!glyphNames) {
      glyphNames = new QStringList();
      GlyphFactory::initFactory();
      Iterator<string> *itS=GlyphFactory::factory->availablePlugins();
      while (itS->hasNext()) {
	glyphNames->append(QString(itS->next().c_str()));
      } delete itS;
    }
    return glyphNames;
  }
public:
  GlyphTableItem(int val) :
    IntFromListTableItem(getGlyphNames(), GLYPHTABLEITEM_RTTI) {
     setValue(val);
  }
  ~GlyphTableItem() {}
  QString valueToText(int val) const;
  int textToValue(const QString& s) const;
};
  
QString GlyphTableItem::valueToText(int val) const {
  return QString(GlyphManager::getInst().glyphName(val).c_str());
}

int GlyphTableItem::textToValue(const QString& s) const {
  return GlyphManager::getInst().glyphId(s.toAscii().data());
}

QStringList* GlyphTableItem::glyphNames = NULL;

//================================================================================
class EdgeShapeTableItem : public IntFromListTableItem {
  static QStringList* edgeShapeNames;
  QStringList* getEdgeShapeNames() {
    if (!edgeShapeNames) {
      edgeShapeNames = new QStringList;
      for (int i = 0; i < GlGraphStaticData::edgeShapesCount; i++)
	edgeShapeNames->append(QString(GlGraphStaticData::edgeShapeName(GlGraphStaticData::edgeShapeIds[i]).c_str()));
    }
    return edgeShapeNames;
  }
      
public:
  EdgeShapeTableItem(int val) :
    IntFromListTableItem(getEdgeShapeNames(), EDGESHAPETABLEITEM_RTTI) {
     setValue(val);
  }
  ~EdgeShapeTableItem() {}
  QString valueToText(int val) const;
  int textToValue(const QString& s) const;
};

QString EdgeShapeTableItem::valueToText(int val) const {
  return QString(GlGraphStaticData::edgeShapeName(val).c_str());
}
  
int EdgeShapeTableItem::textToValue(const QString& s) const {
  return GlGraphStaticData::edgeShapeId(s.toAscii().data());
}

QStringList* EdgeShapeTableItem::edgeShapeNames = NULL;

//================================================================================
class LabelPositionTableItem : public IntFromListTableItem {
  static QStringList* labelPositionNames;
  QStringList* getLabelPositionNames() {
    if (!labelPositionNames) {
      labelPositionNames = new QStringList;
      for (int i = 0; i < 5; i++)
	labelPositionNames->append(QString(GlGraphStaticData::labelPositionName(i).c_str()));
    }
    return labelPositionNames;
  }
      
public:
  LabelPositionTableItem(int val) :
    IntFromListTableItem(getLabelPositionNames(), LABELPOSITIONTABLEITEM_RTTI) {
     setValue(val);
  }
  ~LabelPositionTableItem() {}
  QString valueToText(int val) const;
  int textToValue(const QString& s) const;
};

QString LabelPositionTableItem::valueToText(int val) const {
  return QString(GlGraphStaticData::labelPositionName(val).c_str());
}
  
int LabelPositionTableItem::textToValue(const QString& s) const {
  return GlGraphStaticData::labelPositionId(s.toAscii().data());
}

QStringList* LabelPositionTableItem::labelPositionNames = NULL;

//================================================================================
class SelectionTableItem : public TulipTableWidgetItem {
public:
  SelectionTableItem(bool val);
  ~SelectionTableItem();
  void setValue(bool val) {
    setData(Qt::DisplayRole, QVariant(val));
  }
  QString text() const {
    return QString(data(Qt::DisplayRole).toBool() ? "true" : "false");
  }
};

SelectionTableItem::SelectionTableItem(bool value) :
  TulipTableWidgetItem(SELECTIONTABLEITEM_RTTI) {
  setValue(value);
}
SelectionTableItem::~SelectionTableItem() {}

//================================================================================
class TulipTableItemDelegate :public QItemDelegate {
public:
  TulipTableItemDelegate(TulipTableWidget* parent): QItemDelegate(parent) {}

  ~TulipTableItemDelegate() {}
  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem & option,
			const QModelIndex& index) const;
  void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
  void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const;
};

QWidget* TulipTableItemDelegate::createEditor(QWidget* p, const QStyleOptionViewItem& option,
					      const QModelIndex& index) const {
  TulipTableWidget* table = (TulipTableWidget *) parent();
  QTableWidgetItem* item = table->item(index.row(), index.column());
  if (!item)
    return QItemDelegate::createEditor(p, option, index);
  switch(item->type()) {
  case COLORTABLEITEM_RTTI:
    return ((ColorTableItem *) item)->createEditor(table);
  case COORDTABLEITEM_RTTI:
    return ((CoordTableItem *) item)->createEditor(table);
  case EDGESHAPETABLEITEM_RTTI:
    return ((EdgeShapeTableItem *) item)->createEditor(table);
  case FILETABLEITEM_RTTI:
    return ((FileTableItem *) item)->createEditor(table);
  case GLYPHTABLEITEM_RTTI:
    return ((GlyphTableItem *) item)->createEditor(table);
  case LABELPOSITIONTABLEITEM_RTTI:
    return ((LabelPositionTableItem *) item)->createEditor(table);
  case SIZETABLEITEM_RTTI:
    return ((SizeTableItem *) item)->createEditor(table->viewport());
  default:
    return QItemDelegate::createEditor(p, option, index);
  }
}

void TulipTableItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem& option,
				   const QModelIndex& index) const {
  TulipTableWidget* table = (TulipTableWidget *) parent();
  QTableWidgetItem* item = table->item(index.row(), index.column());
  if (!item) {
    QItemDelegate::paint(painter, option, index);
    return;
  }
  switch(item->type()) {
  case COLORTABLEITEM_RTTI:
    painter->fillRect(option.rect, QColor(((ColorTableItem*) item)->getColor()));
    break;
  default:
    QItemDelegate::paint(painter, option, index);
  }
}

void TulipTableItemDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
					  const QModelIndex& index ) const {
  TulipTableWidget* table = (TulipTableWidget *) parent();
  QTableWidgetItem* item = table->item(index.row(), index.column());
  switch(item->type()) {
  case COLORTABLEITEM_RTTI:
    ((ColorTableItem*) item)->setContentFromEditor(editor);
    break;
  case COORDTABLEITEM_RTTI:
    ((CoordTableItem *) item)->setContentFromEditor(editor);
    break;
  case EDGESHAPETABLEITEM_RTTI:
    ((EdgeShapeTableItem *) item)->setContentFromEditor(editor);
    break;
  case FILETABLEITEM_RTTI:
    ((FileTableItem *) item)->setContentFromEditor(editor);
    break;
  case GLYPHTABLEITEM_RTTI:
    ((GlyphTableItem *) item)->setContentFromEditor(editor);
    break;
  case LABELPOSITIONTABLEITEM_RTTI:
    ((LabelPositionTableItem *) item)->setContentFromEditor(editor);
    break;
  case SIZETABLEITEM_RTTI:
    ((SizeTableItem*) item)->setContentFromEditor(editor);
    break;
  default:
    QItemDelegate::setModelData(editor, model, index);
  }
}
  
TulipTableWidget::TulipTableWidget(QWidget *parent, const char *name) :
  QTableWidget(parent), updateColumnTitle(false) {
  resetBackColor1();
  resetBackColor2();
  setItemDelegate(new TulipTableItemDelegate(this));
}

TulipTableWidget::~TulipTableWidget() {}

bool TulipTableWidget::getUpdateColumnTitle() const {return updateColumnTitle;}
void TulipTableWidget::setUpdateColumnTitle(const bool b) { updateColumnTitle = b;}

QColor TulipTableWidget::getBackColor1() const {return backColor1;}
void TulipTableWidget::setBackColor1(const QColor &c) {backColor1 = c;}
void TulipTableWidget::resetBackColor1() {backColor1 = QColor(236, 245, 255);}

QColor TulipTableWidget::getBackColor2() const {return backColor2;}
void TulipTableWidget::setBackColor2(const QColor &c) {backColor2 = c;}
void TulipTableWidget::resetBackColor2() {backColor2 = QColor(250, 250, 250);}

QColor TulipTableWidget::backgroundColor(const int row) const {
  return (row % 2) ? backColor1 : backColor2;
}

void TulipTableWidget::setTulipNodeItem(const PropertyInterface *editedProperty,
					const std::string propertyName, const node &n,
					const int row, const int col) {
  QString label;
  if (propertyName == "viewShape") {
    int shapenum = ((IntegerProperty *)editedProperty)->getNodeValue(n);
    setItem(row, col, new GlyphTableItem(shapenum));
  } else if (propertyName == "viewLabelPosition") {
    int labelPos = ((IntegerProperty *)editedProperty)->getNodeValue(n);
    /* LabelPositionTableItem *item = new LabelPositionTableItem(this, false);

    label = tr("Label");
    this->setItem(row, col, item);
    item->setCurrentItem(GlGraphStaticData::labelPositionName(labelPos).c_str()); */

    setItem(row, col, new LabelPositionTableItem(labelPos));
  }
  else if (propertyName == "viewTexture") {
    setItem(row, col,
	    new FileTableItem(QString(const_cast<PropertyInterface *>(editedProperty)->getNodeStringValue(n).c_str())));
  }
  else if (typeid(*editedProperty) == typeid(BooleanProperty)) {
    setItem(row, col, new SelectionTableItem(((BooleanProperty *)editedProperty)->getNodeValue(n)));
  }
  else if (typeid(*editedProperty) == typeid(ColorProperty)) {
    ColorProperty *tmpCol = (ColorProperty *)editedProperty;
    Color c = tmpCol->getNodeValue(n);    
    setItem(row, col, new ColorTableItem(qRgba(c[0], c[1], c[2], c[3])));
  }
  else if (typeid(*editedProperty) == typeid(SizeProperty)) {
    SizeProperty *tmpSiz = (SizeProperty *) editedProperty;
    Size s = tmpSiz->getNodeValue(n);
    setItem(row, col, new SizeTableItem(s));
  }
  else if (typeid(*editedProperty) == typeid(LayoutProperty)) {
    LayoutProperty *tmpLay = (LayoutProperty *)editedProperty;
    Coord c = tmpLay->getNodeValue(n);
    setItem(row, col, new CoordTableItem(c));
  }
  else {
    setItem(row, col,
	    new TulipTableWidgetItem(QString(const_cast<PropertyInterface *>(editedProperty)->getNodeStringValue(n).c_str())));
  }
  setRowHeight(row, ROW_HEIGHT);
  if (updateColumnTitle) {
    horizontalHeaderItem(col)->setText(label);
  }
}

void TulipTableWidget::setTulipEdgeItem(const PropertyInterface *editedProperty, const std::string propertyName, const edge &e,
                                    const int row, const int col) {
  QString label;
  if (typeid(*editedProperty) == typeid(BooleanProperty)) {
    setItem(row, col, new SelectionTableItem(((BooleanProperty *)editedProperty)->getEdgeValue(e)));
  }
  else if (typeid(*editedProperty) == typeid(ColorProperty)) {
    ColorProperty *tmpCol = (ColorProperty *)editedProperty;
    Color c = tmpCol->getEdgeValue(e);
    setItem(row, col, new ColorTableItem(qRgba(c[0], c[1], c[2], c[3])));
  }
  else if (typeid(*editedProperty) == typeid(SizeProperty)) {
    SizeProperty *tmpSiz = (SizeProperty *)editedProperty;
    Size s = tmpSiz->getEdgeValue(e);
    setItem(row, col, new SizeTableItem(s));
  } else if (propertyName == "viewShape") {
    int shapenum = ((IntegerProperty *)editedProperty)->getEdgeValue(e);
    setItem(row, col, new EdgeShapeTableItem(shapenum));
  }
//   else if (typeid(*editedProperty) == typeid(Layout)) {
//     Layout *tmpLay = getProperty<LayoutProperty>(graph, propertyName);
//     Coord c = tmpLay->getEdgeValue(e);
//     CoordTableItem *item = new CoordTableItem(this);
//     item->setCoord(c);
//     label = tr("Coord");
//     this->setItem(row, col, item);
//   }
  else {
    /*QTableItem *item = new QTableItem(this, QTableItem::OnTyping,
                                      const_cast<PropertyInterface *>(editedProperty)->getEdgeStringValue(e).c_str() );
    label = tr("Value");
    this->setItem(row, col, item);*/
    setItem(row, col,
	    new TulipTableWidgetItem(QString(const_cast<PropertyInterface *>(editedProperty)->getEdgeStringValue(e).c_str())));
    
  }
  setRowHeight(row, ROW_HEIGHT);
  if (updateColumnTitle) {
    //this->horizontalHeader()->setLabel(col, label);
    horizontalHeaderItem(col)->setText(label);
  }
}
