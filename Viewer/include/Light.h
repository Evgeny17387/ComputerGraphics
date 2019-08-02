#pragma once

#include "MeshModel.h"
#include <glm/glm.hpp>

class Light{

public:

	MeshModel * lightModel;

	int _sourceType = 0;

	glm::vec4 _color = glm::vec4(1, 1, 1, 1);

	Light(int sourceType);

	int _shiness = 100;

	// Object

	void getObjParameters(Object &obj, glm::vec4 &color, int &sourceType);
	void setObjParameters(Object obj, glm::vec4 color);

	glm::mat4x4 getObjTransform();

	glm::vec3 getPosition();

	glm::vec3 getDirection();

};
