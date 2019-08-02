#pragma once
#include "Object.h"
#include <math.h>

using namespace std;

Object::Object(){

}

Object::~Object(){

}

void Object::resetObjParameters(){
	_xPosition = 0;
	_yPosition = 0;
	_zPosition = 0;
	_theta = 0;
	_phi = 0;
	_zeta = 0;
	_zoom = 0.5;
}

void Object::resetWorldParameters(){
	_xPositionWorld = 0;
	_yPositionWorld = 0;
	_zPositionWorld = 0;
	_thetaWorld = 0;
	_phiWorld = 0;
	_zetaWorld = 0;
}

glm::mat4x4 Object::getRotationMatrix(int axis, int angle){

	glm::mat4x4 rotationMatrix;

	switch (axis){

		case 1:

			rotationMatrix = glm::mat4x4(
											cos(angle*PI/180.0), sin(angle*PI/180.0), 0, 0,
											-sin(angle*PI/180.0), cos(angle*PI/180.0), 0, 0,
											0, 0, 1, 0,
											0, 0, 0, 1
														);
			break;

		case 2:

			rotationMatrix = glm::mat4x4(
											1, 0, 0, 0,
											0, cos(angle*PI/180.0), sin(angle*PI/180.0), 0,
											0, -sin(angle*PI/180.0), cos(angle*PI/180.0), 0,
											0, 0, 0, 1
														);
			break;

		case 3:

			rotationMatrix = glm::mat4x4(
											cos(angle*PI/180.0), 0, sin(angle*PI/180.0), 0,
											0, 1, 0, 0,
											-sin(angle*PI/180.0), 0, cos(angle*PI/180.0), 0,
											0, 0, 0, 1
														);
			break;

	}

	return rotationMatrix;

}

glm::mat4x4 Object::getRotationMatrix(int relative){
	
	glm::mat4x4 rotationMatrix;

	switch (relative){
		case 1:
			rotationMatrix = getRotationMatrix(3, _zeta) * getRotationMatrix(2, _phi) * getRotationMatrix(1, _theta);
			break;
		case 2:
			rotationMatrix = getRotationMatrix(3, _zetaWorld) * getRotationMatrix(2, _phiWorld) * getRotationMatrix(1, _thetaWorld);
			break;
	}

	return rotationMatrix;

}

glm::mat4x4 Object::getMoveMatrix(float xPosition, float yPosition, float zPosition){

	glm::mat4x4 moveMatrix = glm::mat4x4(
									1, 0, 0, 0,
									0, 1, 0, 0,
									0, 0, 1, 0,
									xPosition, yPosition, zPosition, 1
												);

	return moveMatrix;

}

glm::mat4x4 Object::getMoveMatrix(int relative){
	
	glm::mat4x4 moveMatrix;

	switch (relative){
		case 1:
			moveMatrix = getMoveMatrix(_xPosition, _yPosition, _zPosition);
			break;
		case 2:
			moveMatrix = getMoveMatrix(_xPositionWorld, _yPositionWorld, _zPositionWorld);
			break;
	}

	return moveMatrix;

}


glm::mat4x4 Object::getScaleMatrix(){
	
	glm::mat4x4 scaleMatrix = getScaleMatrix(_zoom);

	return scaleMatrix;

}

glm::mat4x4 Object::getScaleMatrix(float zoom){

	glm::mat4x4 scaleMatrix = glm::mat4x4(
									zoom, 0, 0, 0,
									0, zoom, 0, 0,
									0, 0, zoom, 0,
									0, 0, 0, 1
												);

	return scaleMatrix;

}
