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
#ifndef ABSTRACTGLCURVE_H
#define ABSTRACTGLCURVE_H

#include <tulip/GlSimpleEntity.h>
#include <tulip/Coord.h>
#include <tulip/Color.h>
#include <tulip/GlShaderProgram.h>

namespace tlp {

class TLP_GL_SCOPE AbstractGlCurve : public GlSimpleEntity {

public :

  AbstractGlCurve(const std::string &shaderProgramName, const std::string &curveSpecificShaderCode);

  AbstractGlCurve(const std::string &shaderProgramName, const std::string &curveSpecificShaderCode, const std::vector<Coord> &controlPoints,
                  const Color &startColor, const Color &endColor, const float startSize, const float endSize, const unsigned int nbCurvePoints);

  virtual ~AbstractGlCurve();

  void draw(float lod, Camera *camera);

  void translate(const Coord &move);

  virtual void setTexture(const std::string &texture) {
    this->texture = texture;
  }

  virtual void setOutlined(const bool outlined) {
    this->outlined = outlined;
  }

  virtual void setOutlineColor(const Color &outlineColor) {
    this->outlineColor = outlineColor;
  }

  virtual void setBillboardCurve(const bool billboardCurve) {
    this->billboardCurve = billboardCurve;
  }

  virtual void setLookDir(const Coord &lookDir) {
    this->lookDir = lookDir;
  }

  void getXML(xmlNodePtr rootNode);

  void setWithXML(xmlNodePtr rootNode);

  virtual void drawCurve(std::vector<Coord> &controlPoints, const Color &startColor, const Color &endColor, const float startSize, const float endSize, const unsigned int nbCurvePoints=100);

protected:

  virtual void setCurveVertexShaderRenderingSpecificParameters() {};

  virtual void cleanupAfterCurveVertexShaderRendering() {};

  virtual Coord computeCurvePointOnCPU(const std::vector<Coord> &controlPoints, float t) = 0;

  virtual void computeCurvePointsOnCPU(const std::vector<Coord> &controlPoints, std::vector<Coord> &curvePoints, unsigned int nbCurvePoints) = 0;

  static void buildCurveVertexBuffers(const unsigned int nbCurvePoints, bool vboOk);

  void initShader(const std::string &shaderProgramName, const std::string &curveSpecificShaderCode);

  static std::map<unsigned int, GLfloat *> curveVertexBuffersData;
  static std::map<unsigned int, std::vector<GLushort *> > curveVertexBuffersIndices;
  static std::map<unsigned int, GLuint* > curveVertexBuffersObject;
  // TODO : remove this static variable in Tulip 3.7 (if there is such a release in the future)
  // can not remove it in Tulip 3.6.1 otherwise binary compatibility will be broken
  static GLint MAX_SHADER_CONTROL_POINTS;
  static std::map<std::string, GlShaderProgram *> curvesShadersMap;
  static std::map<std::string, GlShaderProgram *> curvesBillboardShadersMap;
  static GlShader *curveVertexShaderNormalMain;
  static GlShader *curveVertexShaderBillboardMain;
  static GlShader *fisheyeDistortionVertexShader;
  static bool canUseTBO;

  std::string shaderProgramName;
  GlShaderProgram *curveShaderProgramNormal;
  GlShaderProgram *curveShaderProgramBillboard;
  GlShaderProgram *curveShaderProgram;


  std::vector<Coord> controlPoints;
  Color startColor;
  Color endColor;
  float startSize;
  float endSize;
  unsigned int nbCurvePoints;
  bool outlined;
  Color outlineColor;
  std::string texture;
  float texCoordFactor;
  bool billboardCurve;
  Coord lookDir;

};

}

#endif

