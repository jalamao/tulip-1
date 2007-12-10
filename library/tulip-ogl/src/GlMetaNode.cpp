#include "tulip/GlMetaNode.h"

#include <GL/gl.h>


#include <tulip/Graph.h>
#include <tulip/GraphProperty.h>
#include <tulip/LayoutProperty.h>
#include <tulip/SizeProperty.h>
#include <tulip/DoubleProperty.h>
#include <tulip/IntegerProperty.h>
#include <tulip/ColorProperty.h>

#include "tulip/Glyph.h"
#include "tulip/GlCPULODCalculator.h"
#include "tulip/GlGraphInputData.h"
#include "tulip/DrawingTools.h"
#include "tulip/GlEdge.h"
#include "tulip/GlComplexeEntity.h"

#include <iostream>

using namespace std;

namespace tlp {

  static const Color colorSelect2 = Color(255, 102, 255, 255);

  void GlMetaNode::draw(float lod,GlGraphInputData* data,Camera* camera) {
    node n=node(id);
    if(((data->elementColor->getNodeValue(n))[3]==255) && (data->parameters->getNodesStencil()==0xFFFF)){
      GlNode::draw(lod,data,camera);
      return;
    }
    //glDepthFunc(GL_LESS);
    glPushMatrix();
    const Coord &nodeCoord = data->elementLayout->getNodeValue(node(id));
    const Size &nodeSize = data->elementSize->getNodeValue(node(id));
    glTranslatef(nodeCoord[0], nodeCoord[1], nodeCoord[2]);
    glRotatef(data->elementRotation->getNodeValue(node(id)), 0., 0., 1.);
    glScalef(nodeSize[0], nodeSize[1], nodeSize[2]);

    Graph *metaGraph = data->elementGraph->getNodeValue(node(id));
    GlGraphRenderingParameters metaParameters;
    metaParameters.setTexturePath(data->parameters->getTexturePath());
    metaParameters.setNodesStencil(data->parameters->getNodesStencil());
    metaParameters.setMetaNodesStencil(data->parameters->getMetaNodesStencil());
    metaParameters.setEdgesStencil(data->parameters->getEdgesStencil());
    GlGraphInputData metaData(metaGraph,&metaParameters);
    pair<Coord, Coord> bboxes = tlp::computeBoundingBox(metaData.getGraph(), metaData.elementLayout, metaData.elementSize, metaData.elementRotation);
    //  cerr << bboxes.first << "/" << bboxes.second << endl;
    Coord maxC = bboxes.first;
    Coord minC = bboxes.second;
    //MatrixGL saveMatrix(modelviewMatrix);
    BoundingBox includeBoundingBox;
    data->glyphs.get(data->elementShape->getNodeValue(n))->getIncludeBoundingBox(includeBoundingBox);
    Coord includeScale=includeBoundingBox.second-includeBoundingBox.first;
    Coord size=(maxC + minC)/-1.;
    Coord translate=(maxC+minC)/-2 - (maxC-minC) + includeBoundingBox.first*((maxC-minC)*2) +(maxC-minC)*includeScale ;
    double dept  = (maxC[2] - minC[2]) / includeScale[2];
    double width  = (maxC[0] - minC[0]) / includeScale[0];
    double height = (maxC[1] - minC[1]) / includeScale[1];
    if (width<0.0001) width=1;
    if (height<0.0001) height=1;
    if (dept<0.0001) dept=1;
    Camera newCamera=*camera;

    Coord scale(1/width,1/height,1/dept);
    
    vector<GlNode> nodes;
    vector<GlMetaNode> metaNodes;
    vector<GlEdge> edges;

    Iterator<node> *itN=metaGraph->getNodes();
    unsigned int id;
    while (itN->hasNext()) {
       id=itN->next().id;
      if(data->elementGraph->getNodeValue(node(id)) == 0)
	nodes.push_back(GlNode(id));
      else
	metaNodes.push_back(GlMetaNode(id));
 }
    delete itN;

    if (data->parameters->isDisplayEdges()) {
      Iterator<edge> *itE=metaGraph->getEdges();
      while (itE->hasNext()) {
	edges.push_back(GlEdge(itE->next().id));
      }
      delete itE;
    }

    GlCPULODCalculator calculator;

    calculator.beginNewCamera(&newCamera);

    for(vector<GlNode>::iterator it=nodes.begin();it!=nodes.end();++it) {
      BoundingBox bb = (*it).getBoundingBox(data);
      Coord size=bb.second-bb.first;
      Coord middle=bb.first+size/2;

      middle+=nodeCoord+translate;
      size=size*nodeSize*scale;
      
      /*bb.first=bb.first+nodeCoord+translate;
	bb.second=bb.second+nodeCoord+translate;*/
      bb.first=middle-size/2;
      bb.second=middle+size/2;
      calculator.addComplexeEntityBoundingBox((unsigned int)(&(*it)),bb);
    }

    for(vector<GlMetaNode>::iterator it=metaNodes.begin();it!=metaNodes.end();++it) {
      BoundingBox bb = (*it).getBoundingBox(data);
      Coord size=bb.second-bb.first;
      Coord middle=bb.first+size/2;

      middle+=nodeCoord+translate;
      size=size*nodeSize*scale;
      
      /*bb.first=bb.first+nodeCoord+translate;
	bb.second=bb.second+nodeCoord+translate;*/
      bb.first=middle-size/2;
      bb.second=middle+size/2;
      calculator.addComplexeEntityBoundingBox((unsigned int)(&(*it)),bb);
    }

    if (data->parameters->isDisplayEdges()) {
      for(vector<GlEdge>::iterator it=edges.begin();it!=edges.end();++it) {
	BoundingBox bb = (*it).getBoundingBox(data);
	Coord size=bb.second-bb.first;
	Coord middle=bb.first+(size)/2;
	
	middle+=nodeCoord+translate;
	size=size*nodeSize*scale;
	
	bb.first=bb.first+nodeCoord+translate;
	bb.second=bb.second+nodeCoord+translate;
	calculator.addComplexeEntityBoundingBox((unsigned int)(&(*it)),(*it).getBoundingBox(data));
      }
    }

    calculator.compute(camera->getViewport());
    
    LODResultVector* result=calculator.getResultForComplexeEntities();
    
    for(LODResultVector::iterator it=result->begin();it!=result->end();++it) {
      glPushMatrix();
      glScalef(1.0/width, 1.0/height, 1.0/dept);
      glTranslatef(translate[0],translate[1],translate[2]);
      for(std::vector<LODResultEntity>::iterator itM=(*it).second.begin();itM!=(*it).second.end();++itM) {
	((GlComplexeEntity*)(*itM).first)->draw((*itM).second,&metaData,camera);
      }
      glPopMatrix();
    }

    glPopMatrix();

    GlNode::draw(lod,data,camera);
  }
}
