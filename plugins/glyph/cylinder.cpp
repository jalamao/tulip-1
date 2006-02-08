#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>

#include <tulip/StringProxy.h>
#include <tulip/ColorsProxy.h>

#include <tulip/SuperGraph.h>
#include <tulip/GlGraph.h>
#include <tulip/Glyph.h>

using namespace std;

class Cylinder : public Glyph {
public:
  Cylinder(GlyphContext *gc=NULL);
  virtual ~Cylinder();
  virtual string getName() {return string("Cylinder");}
  virtual void draw(node n);
  virtual Coord getAnchor(const Coord &vector) const;
  virtual void setLOD(int n);

private:
  GLuint LList;
  bool listOk;
};

GLYPHPLUGIN(Cylinder, "Cylinder", "Bertrand Mathieu", "31/07/2002", "Textured Cylinder", "1", "1", 6);

//=================================================================================================
Cylinder::Cylinder(GlyphContext *gc): Glyph(gc),listOk(false) {
  setLOD(8);
}

Cylinder::~Cylinder() {
  if (listOk)
    if (glIsList(LList)) glDeleteLists(LList, 1);
}

void Cylinder::setLOD(int n) {
  LOD = ((n<0) ? 0 : ((n > 10) ? 10 : n));
  if (listOk) {glDeleteLists(LList, 1);listOk=false;};
}

void Cylinder::draw(node n) {
  setMaterial(glGraph->elementColor->getNodeValue(n));
  string texFile = glGraph->elementTexture->getNodeValue(n);
  if (texFile != "") {
    if (glGraph->activateTexture(texFile))
      setMaterial(Color(255,255,255,255));
  }

  if (!listOk) {
    GLUquadricObj *quadratic;
    quadratic = gluNewQuadric();
    gluQuadricNormals(quadratic, GLU_SMOOTH);
    gluQuadricTexture(quadratic, GL_TRUE);  
    LList = glGenLists( 1 );
    glNewList( LList, GL_COMPILE ); 
    glTranslatef(0.0f, 0.0f, -0.5f);
    gluQuadricOrientation(quadratic, GLU_INSIDE);
    gluDisk(quadratic, 0.0f, 0.5f, LOD, LOD);
    gluQuadricOrientation(quadratic, GLU_OUTSIDE);
    gluCylinder(quadratic, 0.5f, 0.5f, 1.0f, LOD, LOD);  
    glTranslatef(0.0f, 0.0f,1.0f);
    gluDisk(quadratic, 0.0f, 0.5f, LOD, LOD);
    glEndList();
    gluDeleteQuadric(quadratic);
    listOk=true;
  }
  glCallList(LList);
}

Coord Cylinder::getAnchor(const Coord &vector) const
{
  Coord anchor = vector;
  float x,y,z,n;
  anchor.get(x,y,z);
  n = sqrt(x*x + y*y);
  if( n == 0.0f )
  	return anchor;
  n = 0.5f / n;
  x *= n;
  y *= n;
  z *= n;
  if (z < -0.5f) z = -0.5f; //z = z >? -0.5f;
  if (z > +0.5f) z = +0.5f; //z = z <? +0.5f;
  return Coord(x,y,z);
}


