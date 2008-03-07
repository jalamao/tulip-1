//*********************************************************************************
//** GlQuad.cpp : Source for a general quad display class
//** 
//** author : Delorme Maxime
//** date   : 07/07/05
//*********************************************************************************
#include "tulip/GlQuad.h"

using namespace std;
using namespace tlp;

GlQuad::GlQuad()
{
  // No default constructor :)
}

void GlQuad::setPosition(const Coord& position)
{
  // No setPosition
}

void GlQuad::setColor(const Color& color)
{
  for(int i=0; i < N_QUAD_POINTS; i++) {
    *(this->colors[i]) = color;
  }
}

GlQuad::GlQuad(Coord positions[N_QUAD_POINTS], const Color &color)
{
  for(int i=0; i < N_QUAD_POINTS; i++)
    {
      this->positions[i] = new Coord(positions[i]);
      this->colors[i]    = new Color(color);
      boundingBox.check(this->position[i]);
    }
}

GlQuad::GlQuad(Coord positions[N_QUAD_POINTS], Color colors[N_QUAD_POINTS])
{
  for(int i=0; i < N_QUAD_POINTS; i++) {
    this->positions[i] = new Coord(positions[i]);
    this->colors[i]    = new Color(colors[i]);

    boundingBox.check(this->position[i]);
  }   
}

GlQuad::~GlQuad()
{
  for(int i=0; i < N_QUAD_POINTS; i++)
    {
      delete positions[i];
      delete colors[i];
    }
}

void GlQuad::setPosition(int idPosition, const Coord &position)
{
  if (idPosition < 0 || idPosition >= N_QUAD_POINTS)
    return;

  delete this->positions[idPosition];

  this->positions[idPosition] = new Coord(position);
}

void GlQuad::setColor(int idColor, const Color &color)
{
  if (idColor < 0 || idColor >= N_QUAD_POINTS)
    return;

  delete this->colors[idColor];

  this->colors[idColor] = new Color(color);
}

Coord* GlQuad::getPosition(int idPosition) const
{
  if (idPosition < 0 || idPosition >= N_QUAD_POINTS)
    return NULL;

  return positions[idPosition];
}

Color* GlQuad::getColor(int idColor) const
{
  if (idColor < 0 || idColor >= N_QUAD_POINTS)
    return NULL;

  return colors[idColor];
}

void GlQuad::draw(float lod, Camera *camera)
{

  GLfloat* cols[N_QUAD_POINTS];

  for(int i=0; i < N_QUAD_POINTS; i++)
    cols[i] = colors[i]->getGL();

  glEnable(GL_COLOR_MATERIAL);
  glBegin(GL_QUADS);
  
  for(int i=0; i < N_QUAD_POINTS; i++){
    glColor4fv(cols[i]);
    glVertex3f(positions[i]->getX(), positions[i]->getY(), positions[i]->getZ());
  }
  
  glEnd();
 
  /*  
  if (renderOptions.getRenderState(GlAD_Wireframe))
    {
	
      if (Solid)
	{
	  GLfloat colBk[4] = {0.0f, 0.0f, 0.0f, 255.0f};

	  for (int i=0; i < N_QUAD_POINTS; i++)
	    cols[i] = colBk;
	}

      glBegin(GL_LINE_STRIP);
	   
      for(int i=0; i <= N_QUAD_POINTS; i++)
	{
	  int id = i % N_QUAD_POINTS;
	    
	  glColor4fv(cols[id]);
	  glVertex3f(positions[id]->getX(), positions[id]->getY(), positions[id]->getZ());
	}
	
      glEnd();
      */
}
//===========================================================
void GlQuad::translate(const Coord& mouvement) {
  boundingBox.first+=mouvement;
  boundingBox.second+=mouvement;
  
  for(int i=0; i < N_QUAD_POINTS; i++) {
    positions[i]+=mouvement;
  }
}
//===========================================================
void GlQuad::getXML(xmlNodePtr rootNode) {
  xmlNodePtr dataNode=NULL;
  
  xmlNewProp(rootNode,BAD_CAST "type",BAD_CAST "GlQuad");
    
    GlXMLTools::getDataNode(rootNode,dataNode);

    GlXMLTools::getXML(dataNode,"position0",positions[0]);
    GlXMLTools::getXML(dataNode,"position1",positions[1]);
    GlXMLTools::getXML(dataNode,"position2",positions[2]);
    GlXMLTools::getXML(dataNode,"position3",positions[3]);
    GlXMLTools::getXML(dataNode,"color0",colors[0]);
    GlXMLTools::getXML(dataNode,"color1",colors[1]);
    GlXMLTools::getXML(dataNode,"color2",colors[2]);
    GlXMLTools::getXML(dataNode,"color3",colors[3]);
    
    
  }
  //============================================================
  void GlBox::setWithXML(xmlNodePtr rootNode) {
    xmlNodePtr dataNode=NULL;

    GlXMLTools::getDataNode(rootNode,dataNode);

    // Parse Data
    if(dataNode) {
      for(int i=0; i < N_QUAD_POINTS; i++) {
	positions[i]=new Coord();
	colors[i]=new Color();
      }  
      GlXMLTools::setWithXML(dataNode,"position0",positions[0]);
      GlXMLTools::setWithXML(dataNode,"position1",positions[1]);
      GlXMLTools::setWithXML(dataNode,"position2",positions[2]);
      GlXMLTools::setWithXML(dataNode,"position3",positions[3]);
      GlXMLTools::setWithXML(dataNode,"color0",colors[0]);
      GlXMLTools::setWithXML(dataNode,"color1",colors[1]);
      GlXMLTools::setWithXML(dataNode,"color2",colors[2]);
      GlXMLTools::setWithXML(dataNode,"color3",colors[3]);

      for(int i=0; i < N_QUAD_POINTS; i++) {
	boundingBox.check(this->position[i]);
      }   
    }
  }
