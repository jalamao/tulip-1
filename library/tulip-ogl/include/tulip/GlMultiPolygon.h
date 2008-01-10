//-*-c++-*-
/**
 Author: Morgan Mathiaut
 Email : mathiaut@labri.fr
 Last modification : 11/12/2007
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by  
 the Free Software Foundation; either version 2 of the License, or     
 (at your option) any later version.
*/
#ifndef Tulip_GLMULTIPOLYGON_H
#define Tulip_GLMULTIPOLYGON_H

#include <vector>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <tulip/tulipconf.h>
#include <tulip/Coord.h>

#include "tulip/GlPolygon.h"
#include "tulip/GlSimpleEntity.h"

namespace tlp {

  class TLP_GL_SCOPE GlMultiPolygon : public GlSimpleEntity {
  
  public:
    
    GlMultiPolygon(bool filled=true,bool outlined=true,const Color& fillColor=Color(0,0,0,255),const Color& outlineColor=Color(0,0,0,255));

    void addPolygon(const std::vector<Coord> &poly);

    virtual void draw(float lod,Camera *camera);

  protected:
    
    std::vector<GlPolygon> polygons;
    
    bool filled;
    bool outlined;
    
    Color fillColor;
    Color outlineColor;

  };
  
}

#endif // Tulip_GLMULTIPOLYGON_H
