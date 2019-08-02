#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"

using namespace std;

Camera::Camera(){

	cameraModel = new MeshModel("/home/evgeny/Desktop/ComputerGraphics/Data/obj/camera.obj");

	cameraModel->_modelColorAmbient = glm::vec4(1.0, 1.0, 1.0, 1.0);

	cameraModel->_obj._yPositionWorld = 2.0;
	cameraModel->_obj._zPositionWorld = 5.0;
	cameraModel->_obj._xPositionWorld = 5.0;
	cameraModel->_obj._zPosition = -2.0;
	cameraModel->_obj._phi = -10.0;
	cameraModel->_obj._zeta = -40.0;

}

// Projection

void Camera::updateProjection(int &fov, float &near1, float &far1, int &projectionType){

	fov = _fov;
	near1 = _near1;
	far1 = _far1;
	projectionType = _projectionType;

}

void Camera::setProjection(int fov, float near1, float far1, int projectionType){

	_fov = fov;
	_near1 = near1;
	_far1 = far1;
	_projectionType = projectionType;

}

const glm::mat4x4& Camera::getProjection(){

	if (_width > _height){

		_right1 = (float)_width/_height;
		_left1 = -(float)_width/_height;

	}else{

		_top1 = (float)_height/_width;
		_buttom1 = -(float)_height/_width;

	}

	if (_projectionType == 0){

		_projection = glm::ortho(_left1, _right1, _buttom1, _top1, _near1, _far1);

	} else if (_projectionType == 1){

		_projection = glm::perspective(glm::radians(_fov), (float) _width / (float)_height, _near1, _far1);
  
	}

	return _projection;

}

// LookAt

glm::mat4x4 Camera::LookAt(const glm::vec3 at){

	glm::mat4 _cTransform = glm::lookAt(
	    glm::vec3( getPosition() + glm::vec3(0.0, 0.0, 0.1) ),
	    at,
	    glm::vec3(0,1,0) 
	);

	return _cTransform;

}

// Object

void Camera::getObjParameters(Object &obj, bool &renderCamera){

	obj = cameraModel->_obj;
	renderCamera = _renderCamera;

}

void Camera::setObjParameters(Object obj, bool renderCamera){

	cameraModel->_obj = obj;
	_renderCamera = renderCamera;

}

glm::mat4x4 Camera::getObjTransform(){

	return cameraModel->_obj.getRotationMatrix(2) * cameraModel->_obj.getMoveMatrix(2) * cameraModel->_obj.getRotationMatrix(1);

}

glm::mat4x4 Camera::getObjTransformScaled(){

	return cameraModel->_obj.getRotationMatrix(2) * cameraModel->_obj.getMoveMatrix(2) * cameraModel->_obj.getScaleMatrix() * cameraModel->_obj.getRotationMatrix(1) * cameraModel->_obj.getRotationMatrix(2, 180) * cameraModel->_obj.getRotationMatrix(1, 180) * cameraModel->_obj.getMoveMatrix(1);

}

glm::vec3 Camera::getPosition(){

	return glm::vec3( cameraModel->_obj.getRotationMatrix(2) * cameraModel->_obj.getMoveMatrix(2) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f) );

}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, int mode){

    xoffset *= _rotationSensitivity;
    yoffset *= _rotationSensitivity;

	if (mode == 0){

	    cameraModel->_obj._zeta += xoffset;
	    cameraModel->_obj._phi += yoffset;

	}else if (mode == 1){

	    cameraModel->_obj._zetaWorld += xoffset;
	    cameraModel->_obj._phiWorld += yoffset;

	}

}

void Camera::ProcessKeyboard(int key, float deltaTime){

	switch (key){
		case 0:
			cameraModel->_obj._xPositionWorld += deltaTime*_moveSensitivity;
		break;
		case 1:
			cameraModel->_obj._xPositionWorld -= deltaTime*_moveSensitivity;
		break;
		case 2:
			cameraModel->_obj._zPositionWorld += deltaTime*_moveSensitivity;
		break;
		case 3:
			cameraModel->_obj._zPositionWorld -= deltaTime*_moveSensitivity;
		break;
		case 4:
			cameraModel->_obj._yPositionWorld += deltaTime*_moveSensitivity;
		break;
		case 5:
			cameraModel->_obj._yPositionWorld -= deltaTime*_moveSensitivity;
		break;

	}

}
