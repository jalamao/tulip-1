#include <cmath>

#include <tulip/StringProperty.h>
#include <tulip/ColorProperty.h>
#include <tulip/DoubleProperty.h>

#include <tulip/Glyph.h>
#include <tulip/EdgeExtremityGlyph.h>
#include <tulip/Graph.h>
#include <tulip/GlBox.h>

using namespace std;
using namespace tlp;

/** \addtogroup glyph */
/*@{*/
/// A 3D glyph.
/** This glyph draws a transparent cube using the "viewBorderColor" node
 * property value to draw its edges.
 */
class CubeOutLinedTransparent: public Glyph,
		public EdgeExtremityGlyphFrom3DGlyph {
public:
	CubeOutLinedTransparent(GlyphContext *gc = NULL);
	CubeOutLinedTransparent(EdgeExtremityGlyphContext *gc = NULL);
	virtual ~CubeOutLinedTransparent();
	virtual void draw(node n, float lod);
	virtual void draw(edge e, node n, const Color& glyphColor, const Color &borderColor, float lod);
  virtual void draw(const Color &borderColor,float borderWidth,const std::string &textureName, float lod);
  virtual Coord getAnchor(const Coord & vector) const;

protected:

  static GlBox* box;

};

#ifdef _WIN32
#ifdef DLL_EXPORT
GlBox* CubeOutLinedTransparent::box=0;
#endif
#else
GlBox* CubeOutLinedTransparent::box=0;
#endif

GLYPHPLUGIN(CubeOutLinedTransparent, "3D - Cube OutLined Transparent", "David Auber", "09/07/2002", "Textured cubeOutLined", "1.0", 9)
;
EEGLYPHPLUGIN(CubeOutLinedTransparent, "3D - Cube OutLined Transparent", "David Auber", "09/07/2002", "Textured cubeOutLined", "1.0", 10)
;

//===================================================================================
CubeOutLinedTransparent::CubeOutLinedTransparent(GlyphContext *gc) :
	Glyph(gc), EdgeExtremityGlyphFrom3DGlyph(NULL) {
  if(!box)
    box = new GlBox(Coord(0,0,0),Size(1,1,1),Color(0,0,0,255),Color(0,0,0,255));
}
CubeOutLinedTransparent::CubeOutLinedTransparent(EdgeExtremityGlyphContext *gc) :
	Glyph(NULL), EdgeExtremityGlyphFrom3DGlyph(gc) {
  if(!box)
    box = new GlBox(Coord(0,0,0),Size(1,1,1),Color(0,0,0,255),Color(0,0,0,255));
}

CubeOutLinedTransparent::~CubeOutLinedTransparent() {
}

void CubeOutLinedTransparent::draw(node n, float lod) {
  string textureName=glGraphInputData->elementTexture->getNodeValue(n);
  if(textureName!="")
    textureName=glGraphInputData->parameters->getTexturePath()+textureName;

  draw(glGraphInputData->elementBorderColor->getNodeValue(n),
       glGraphInputData->elementBorderWidth->getNodeValue(n),
       textureName,
       lod);
}

void CubeOutLinedTransparent::draw(edge e, node n, const Color &borderColor, const Color& glyphColor,
    float lod) {
  string textureName=edgeExtGlGraphInputData->elementTexture->getEdgeValue(e);
  if(textureName!="")
    textureName=edgeExtGlGraphInputData->parameters->getTexturePath()+textureName;

  draw(borderColor,
       edgeExtGlGraphInputData->elementBorderWidth->getEdgeValue(e),
       textureName,
       lod);
	glDisable(GL_LIGHTING);
}

void CubeOutLinedTransparent::draw(const Color &borderColor,float borderWidth,const std::string &textureName, float lod){
  if (textureName.size() != 0){
    const string& texturePath=glGraphInputData->parameters->getTexturePath();
    box->setTextureName(texturePath+textureName);
  }else
    box->setTextureName("");

  box->setFillColor(Color(0,0,0,0));
  box->setOutlineColor(borderColor);
  double lineWidth=borderWidth;
  if(lineWidth < 1e-6)
    lineWidth=1e-6;
  box->setOutlineSize(lineWidth);

  box->draw(lod,NULL);
}

Coord CubeOutLinedTransparent::getAnchor(const Coord & vector) const {
	float x, y, z, fmax;
	vector.get(x, y, z);
	fmax = std::max(std::max(fabsf(x), fabsf(y)), fabsf(z));
	if (fmax > 0.0f)
		return vector * (0.5f / fmax);
	else
		return vector;
}
