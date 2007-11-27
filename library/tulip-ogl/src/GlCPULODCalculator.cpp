#include "tulip/GlCPULODCalculator.h"

#include <GL/glu.h>
#include <GL/gl.h>

#include <tulip/Matrix.h>

#include "tulip/Camera.h"
#include "tulip/GlEntity.h"
#include "tulip/GlTools.h"

#include <iostream>

using namespace std;

namespace tlp {

  GlCPULODCalculator::~GlCPULODCalculator() {
    for(CameraAndBoundingBoxVector::iterator it=boundingBoxVector.begin();it!=boundingBoxVector.end();++it) {
      delete (*it).second.first;
      delete (*it).second.second;
    }
  }

  void GlCPULODCalculator::beginNewCamera(Camera* camera) {
    actualSEBoundingBoxVector=new BoundingBoxVector();
    actualCEBoundingBoxVector=new BoundingBoxVector();

    pair<BoundingBoxVector*,BoundingBoxVector*> boundingBoxPair=pair<BoundingBoxVector*,BoundingBoxVector*>(actualSEBoundingBoxVector,actualCEBoundingBoxVector);
    boundingBoxVector.push_back(pair<unsigned int ,pair<BoundingBoxVector*,BoundingBoxVector*> >((unsigned int)camera,boundingBoxPair));
  }

  void GlCPULODCalculator::addSimpleEntityBoundingBox(unsigned int entity,const BoundingBox& bb) {
    actualSEBoundingBoxVector->push_back(BoundingBoxUnit(entity,bb));
  }
  void GlCPULODCalculator::addComplexeEntityBoundingBox(unsigned int entity,const BoundingBox& bb) {
    actualCEBoundingBoxVector->push_back(BoundingBoxUnit(entity,bb));
  }
  
  void GlCPULODCalculator::compute(const Vector<int,4>& viewport) {
    for(CameraAndBoundingBoxVector::iterator itM=boundingBoxVector.begin();itM!=boundingBoxVector.end();++itM) {
      		       
      Camera *camera=(Camera*)((*itM).first);

      seResultVector.push_back(LODResultCameraAndEntities((*itM).first,vector<LODResultEntity>()));
      ceResultVector.push_back(LODResultCameraAndEntities((*itM).first,vector<LODResultEntity>()));
      LODResultVector::iterator itSE=--seResultVector.end();
      LODResultVector::iterator itCE=--ceResultVector.end();
      
      Matrix<float,4> modelviewMatrix;
      Matrix<float,4> projectionMatrix;

      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();

      float ratio = double(viewport[2])/double(viewport[3]);
    
      if (ratio>1)
	glOrtho(ratio*-camera->getSceneRadius()/2.0/camera->getZoomFactor(), ratio*camera->getSceneRadius()/2.0/camera->getZoomFactor(),
		-camera->getSceneRadius()/2.0/camera->getZoomFactor(), camera->getSceneRadius()/2.0/camera->getZoomFactor(),
		-100, 100);
      else 
	glOrtho(-camera->getSceneRadius()/2.0/camera->getZoomFactor(), camera->getSceneRadius()/2.0/camera->getZoomFactor(),
		1./ratio * - camera->getSceneRadius()/2.0/camera->getZoomFactor(), 1./ratio * camera->getSceneRadius()/2.0/camera->getZoomFactor(),
		-100, 100);
      
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
      glLoadIdentity();
      gluLookAt(camera->getEyes()[0], camera->getEyes()[1], camera->getEyes()[2], 
		camera->getCenter()[0], camera->getCenter()[1], camera->getCenter()[2],
		camera->getUp()[0], camera->getUp()[1], camera->getUp()[2]); 
      
      glGetFloatv (GL_MODELVIEW_MATRIX, (GLfloat*)&modelviewMatrix);
      glGetFloatv (GL_PROJECTION_MATRIX, (GLfloat*)&projectionMatrix);

      for(BoundingBoxVector::iterator itV=(*itM).second.first->begin();itV!=(*itM).second.first->end();++itV){
	(*itSE).second.push_back(pair<unsigned int,float>((*itV).first,projectSize((*itV).second, projectionMatrix, modelviewMatrix, viewport)));
      }
      for(BoundingBoxVector::iterator itV=(*itM).second.second->begin();itV!=(*itM).second.second->end();++itV){
	(*itCE).second.push_back(pair<unsigned int,float>((*itV).first,projectSize((*itV).second, projectionMatrix, modelviewMatrix, viewport)));
      }
      glPopMatrix();
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
    }
  }
  
  LODResultVector* GlCPULODCalculator::getResultForSimpleEntities() {
    return &seResultVector;
  }

  LODResultVector* GlCPULODCalculator::getResultForComplexeEntities() {
    return &ceResultVector;
  }
 
}

