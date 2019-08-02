#pragma once

#include <glm/glm.hpp>
#include "MeshModel.h"

class Camera{

private:

	glm::mat4x4 _projection;

	const float _rotationSensitivity =  0.5f;
	const float _moveSensitivity =  5.0f;

	int _projectionType = 1;
	float _buttom1 = -1;
	float _top1 = 1;
	float _near1 = 0.1;
	float _far1 = 100.0;
	float _left1 = -1;
	float _right1 = 1;
	float _fov = 45.0;

public:

	MeshModel * cameraModel;

	bool _renderCamera = false;

	int  _trackObject = -1;

	int _width;
	int _height;

	Camera();

	// Projection

	void updateProjection(int &fov, float &near1, float &far1, int &projectionType);
	void setProjection(int fov, float near1, float far1, int projectionType);

	const glm::mat4x4& getProjection();

	// LookAt

	glm::mat4x4 LookAt(const glm::vec3 at);

	// Object

	void getObjParameters(Object &obj, bool &renderCamera);
	void setObjParameters(Object obj, bool renderCamera);

	glm::mat4x4 getObjTransform();

	glm::vec3 getPosition();

	glm::mat4x4 getObjTransformScaled();

	void ProcessMouseMovement(float xoffset, float yoffset, int mode);
	void ProcessKeyboard(int key, float deltaTime);

};
