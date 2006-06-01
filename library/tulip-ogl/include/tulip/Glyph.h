//-*-c++-*-
#ifndef GLYPH_H
#define GLYPH_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <tulip/Plugin.h>
#include <tulip/WithParameter.h>
#include <tulip/Size.h>
#include <tulip/Coord.h>
#include <tulip/Color.h>
#include <tulip/GlGraph.h>

namespace tlp {

class Graph;
struct node;

class GlyphContext
{
public:
  Graph **graph;
  GlGraph *glGraph;
  int LOF, LOD;
  ///
  GlyphContext(Graph **s=NULL, GlGraph *glGraph=NULL, int LOF=5, int LOD=5): graph(s), glGraph(glGraph),
									     LOF(LOF), LOD(LOD){}
  ///
  ~GlyphContext(){};
};


class TLP_GL_SCOPE Glyph : public WithParameter {
protected:
  Graph **graph;
  GlGraph *glGraph;
  int LOF; //Level Of Filter
  int LOD; //Level Of Detail
  void setMaterial(const Color &c);
  
public:
  Glyph(GlyphContext *);
  ///
  virtual ~Glyph();
  ///
  virtual std::string getName()=0;
  virtual void draw(node)=0;

  /*
   * return a point where an edge coming from "from" can be attached
   * by default, the point will be on the surface of the largest sphere contained
   * inside the unit cube (before scaling).
   */
  virtual Coord getAnchor(const Coord &nodeCenter, const Coord &from, const Size &scale, const double zRotation) const;

  void setLOF(int);
  int getLOF();

  virtual void setLOD(int);
  int getLOD();
  
protected:
  /*
   * called by public method getAnchor to actually compute the anchor point
   * vector is coordinate of the point to anchor to, relative to nodecenter
   * glyph size is (1,1,1)
   * this is the method to redefine for each glyph where the default 'getAnchor' method
   * is inapropriated
   * Returned value is a vector to be applied to 'nodeCenter' in the public method
   */
  virtual Coord getAnchor(const Coord &vector) const;
};

class GlyphFactory: public Plugin
{
public:
  virtual ~GlyphFactory() {}
  ///
  virtual Glyph *createObject(GlyphContext *gc)=0;
  ///
  virtual int getId() const=0;
};

}

#define GPLUGINFACTORY(T,C,N,A,D,I,V,R,ID,G)       \
class C##T##Factory:public tlp::T##Factory	 \
{                                                \
public:                                          \
  C##T##Factory(){				 \
    tlp::GlGraph::initFactory();			\
    tlp::GlGraph::glyphFactory->getPluginParameters(this);	\
  }       					 \
  string getName() const { return string(N);}	 \
  string getGroup() const { return string(G);}	 \
  string getAuthor() const {return string(A);}	 \
  string getDate() const {return string(D);}	 \
  string getInfo() const {return string(I);}	 \
  string getRelease() const {return string(R);}	 \
  string getVersion() const {return string(V);}	 \
  int    getId() const {return ID;}		 \
  tlp::T * createObject(tlp::GlyphContext *gc)	 \
  {						 \
    C *tmp = new C(gc);				 \
    return ((tlp::T *) tmp);			 \
  }						 \
};                                               \
extern "C" {                                            \
  C##T##Factory C##T##FactoryInitializer;               \
}

#define GLYPHPLUGINOFGROUP(C,N,A,D,I,V,R,ID,G) GPLUGINFACTORY(Glyph,C,N,A,D,I,V,R,ID,G)
#define GLYPHPLUGIN(C,N,A,D,I,V,R,ID) GLYPHPLUGINOFGROUP(C,N,A,D,I,V,R,ID,"") 

#endif //GLYPH_H
