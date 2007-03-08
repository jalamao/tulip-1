#include <cmath>

#include <string>
#include <GL/gl.h>
#include <GL/glu.h>

#include <tulip/StringProperty.h>
#include <tulip/ColorProperty.h>
#include <tulip/DoubleProperty.h>
#include <tulip/Size.h>
#include <tulip/Coord.h>
#include <tulip/Glyph.h>

#include <tulip/Graph.h>
#include <tulip/GlGraph.h>
#include <tulip/GlTools.h>

using namespace std;
using namespace tlp;

/** \addtogroup glyph */
/*@{*/
/// A 2D glyph
/**
 * This glyph draws a textured pentagone using the "viewTexture"
 * node property value. If this property has no value, the pentagone
 * is then colored using the "viewColor" node property value.
 */
class Pentagone : public Glyph {
public:
  Pentagone(GlyphContext *gc=NULL);
  virtual ~Pentagone();
  virtual void draw(node n);

protected:
  GLuint LList;
  bool listOk;
  void drawPentagone();
  void drawPentagoneBorder();
};
//=====================================================
GLYPHPLUGIN(Pentagone, "2D - Pentagone", "David Auber", "09/07/2002", "Textured Pentagone", "1.0", 12);
//===================================================================================
Pentagone::Pentagone(GlyphContext *gc): Glyph(gc),listOk(false) {
}
//=====================================================
Pentagone::~Pentagone() {
  if (listOk)
    if (glIsList(LList)) glDeleteLists(LList, 2);
}
//=====================================================
void Pentagone::draw(node n) {
  setMaterial(glGraph->elementColor->getNodeValue(n));
  string texFile = glGraph->elementTexture->getNodeValue(n);
  if (texFile != "") {
    if (glGraph->activateTexture(texFile))
      setMaterial(Color(255,255,255,0));
  }
  if (!listOk) {
    LList = glGenLists( 2 );
    glNewList( LList, GL_COMPILE ); 
    drawPentagone();
    glEndList();
    glNewList( LList + 1, GL_COMPILE ); 
    drawPentagoneBorder();
    glEndList();
    listOk=true;
  }
  glCallList(LList);
  ColorProperty *borderColor = glGraph->getGraph()->getProperty<ColorProperty>("viewBorderColor");
  DoubleProperty *borderWidth = 0;
  if (glGraph->getRenderingParameters().getGraph()->existProperty ("viewBorderWidth"))
    borderWidth = glGraph->getRenderingParameters().getGraph()->getProperty<DoubleProperty>("viewBorderWidth");
  glGraph->desactivateTexture();
  Color c = borderColor->getNodeValue(n);
  //  setMaterial(c);
  if (borderWidth == 0) glLineWidth(2);
  else {
    double lineWidth = borderWidth->getNodeValue (n);
    if (lineWidth < 1e-6) glLineWidth (1e-6); //no negative borders
    else glLineWidth (lineWidth);
  }
  glDisable(GL_LIGHTING);
  glColor4ub(c[0],c[1],c[2],c[3]);
  glCallList(LList + 1);
  glEnable(GL_LIGHTING);
}

//=====================================================
void Pentagone::drawPentagone() {
  GLUquadricObj *quadratic;
  quadratic = gluNewQuadric();
  gluQuadricNormals(quadratic, GLU_SMOOTH);
  gluQuadricTexture(quadratic, GL_TRUE);  
  gluQuadricOrientation(quadratic, GLU_OUTSIDE);
  gluDisk(quadratic, 0.0f, 0.5f, 5, 1);
  gluQuadricOrientation(quadratic, GLU_INSIDE);
  gluDisk(quadratic, 0.0f, 0.5f, 5, 1);
  gluDeleteQuadric(quadratic);
}

void Pentagone::drawPentagoneBorder() {
  glBegin(GL_LINE_LOOP);
  double alpha = M_PI / 2.;
  double delta = 2. * M_PI/5.0;
  for (unsigned int i = 0; i < 5; ++i) {
    glVertex3f(0.5*cos(alpha), 0.5 * sin(alpha), 0.0);
    alpha += delta;
  }
  glEnd();
}
/*@}*/
