#include "Light.h"

#include <iostream>

using namespace std;

Light::Light(int sourceType){

	_sourceType = sourceType;

	float bulbSize = 0.1;

	lightModel = new MeshModel(bulbSize, -bulbSize, bulbSize, -bulbSize, bulbSize, -bulbSize);

}

// Object

void Light::getObjParameters(Object &obj, glm::vec4 &color, int &sourceType){
	obj = lightModel->_obj;
	color = _color;
	sourceType = _sourceType;
}

void Light::setObjParameters(Object obj, glm::vec4 color){
	lightModel->_obj = obj;
	_color = color;
}

glm::mat4x4 Light::getObjTransform(){

	return lightModel->_obj.getRotationMatrix(2) * lightModel->_obj.getMoveMatrix(1);

}

glm::vec3 Light::getPosition(){
	return glm::vec3(lightModel->_obj.getRotationMatrix(2) * lightModel->_obj.getMoveMatrix(1) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

glm::vec3 Light::getDirection(){
	return glm::vec3(lightModel->_obj.getRotationMatrix(2) * glm::vec4(0.0f, -10.0f, 0.01f, 1.0f));
}
