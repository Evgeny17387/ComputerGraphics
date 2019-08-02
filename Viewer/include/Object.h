#pragma once
#include <glm/glm.hpp>

using namespace std;

#define PI 3.14159265

class Object{

public:

	glm::mat4x4 getRotationMatrix(int axis, int angle);
	glm::mat4x4 getMoveMatrix(float xPosition, float yPosition, float zPosition);
	glm::mat4x4 getScaleMatrix(float zoom);

	float _xPosition;
	float _yPosition;
	float _zPosition;

	int _theta;
	int _phi;
	int _zeta;

	float _zoom;

	float _xPositionWorld;
	float _yPositionWorld;
	float _zPositionWorld;

	int _thetaWorld;
	int _phiWorld;
	int _zetaWorld;

	Object();
	~Object();

	void resetObjParameters();
	void resetWorldParameters();

	glm::mat4x4 getRotationMatrix(int relative);
	glm::mat4x4 getMoveMatrix(int relative);
	glm::mat4x4 getScaleMatrix();

};
