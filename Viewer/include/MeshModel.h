#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include "Object.h"

using namespace std;

class MeshModel{

private:

	void loadOBJ(const char* fileName);

protected:

	void initOpenGL();

	std::vector<glm::vec3> _vertices;
	std::vector<glm::vec2> _uvsPlanar;
	std::vector<glm::vec2> _uvsSpherical;
	std::vector<glm::vec3> _normals;

	const float _rotationSensitivity =  0.5f;
	const float _moveSensitivity =  5.0f;

public:

	GLuint _textureID = 0;

	int _texture = 0;

	std::vector<glm::vec2> _uvs;

	Object _obj = Object();

	GLuint _VertexArrayID;

	int _verticesSize;

	glm::vec4 _modelColorAmbient = glm::vec4(1, 1, 1, 1);
	glm::vec4 _modelColorDiffuse = glm::vec4(1, 1, 1, 1);
	glm::vec4 _modelColorSpecular = glm::vec4(1, 1, 1, 1);

	bool _primitive = false;

	bool _drawNormals = false;

	// Constructors

	MeshModel();
	MeshModel(const char* fileName);
	MeshModel(float xMax, float xMin, float yMax, float yMin, float zMax, float zMin);

	// Object

	void getObjParameters(Object &obj, glm::vec4 &modelColorAmbient, glm::vec4 &modelColorDiffuse, glm::vec4 &modelColorSpecular);
	void setObjParameters(Object obj, glm::vec4 modelColorAmbient, glm::vec4 modelColorDiffuse, glm::vec4 modelColorSpecular);

	glm::mat4x4 getObjTransform();

	glm::vec3 getPosition();

	void ProcessMouseMovement(float xoffset, float yoffset, int mode);
	void ProcessKeyboard(int key, float deltaTime);

};
