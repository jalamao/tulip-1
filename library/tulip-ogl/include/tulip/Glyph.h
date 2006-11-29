//-*-c++-*-
#ifndef GLYPH_H
#define GLYPH_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <tulip/Plugin.h>
#include <tulip/WithParameter.h>
#include <tulip/WithDependency.h>
#include <tulip/Size.h>
#include <tulip/Coord.h>
#include <tulip/Color.h>
#include <tulip/GlGraph.h>

namespace tlp {

  class Graph;
  struct node;

  class GlyphContext {
  public:
    GlGraph *glGraph;
    ///
    GlyphContext(Graph **s=NULL, GlGraph *glGraph=NULL, int LOF=5, int LOD=5): 
      glGraph(glGraph) {
    }
    ///
    ~GlyphContext() {
    }
  };
  //==========================================================
  class TLP_GL_SCOPE Glyph : public WithParameter, public WithDependency {
  public:
    Glyph(GlyphContext *);
    virtual ~Glyph();
    virtual void draw(node)=0;
    /*
     * return a point where an edge coming from "from" can be attached
     * by default, the point will be on the surface of the largest sphere contained
     * inside the unit cube (before scaling).
     */
    virtual Coord getAnchor(const Coord &nodeCenter, const Coord &from,
			    const Size &scale, const double zRotation) const;

 
  protected:
    GlGraph *glGraph;
  
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
    virtual Glyph *createPluginObject(GlyphContext *gc)=0;
    ///
    virtual int getId() const=0;

    static TemplateFactory<GlyphFactory,Glyph,GlyphContext *> *factory;
    static TLP_GL_SCOPE void initFactory() {
      if (!factory) {
	factory = new TemplateFactory<GlyphFactory,Glyph,GlyphContext *>;
	factory->currentLoader = 0;
      }
    }
  };

}

#define GPLUGINFACTORY(T,C,N,A,D,I,V,R,ID,G)     \
class C##T##Factory:public tlp::T##Factory	 \
{                                                \
public:                                          \
  C##T##Factory(){				 \
    initFactory(); 			         \
    factory->registerPlugin(this);	         \
  }       					 \
  string getName() const { return string(N);}	 \
  string getGroup() const { return string(G);}	 \
  string getAuthor() const {return string(A);}	 \
  string getDate() const {return string(D);}	 \
  string getInfo() const {return string(I);}	 \
  string getRelease() const {return string(R);}	 \
  string getVersion() const {return string(V);}	 \
  int    getId() const {return ID;}		 \
  tlp::T * createPluginObject(tlp::GlyphContext *gc)	 \
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
