#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <limits>

#include <glm/glm.hpp>

#include "MeshModel.h"
#include "Utils.h"

using namespace std;

// Private

void MeshModel::loadOBJ(const char* fileName){

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	float minX = numeric_limits<float>::max();
	float minY = numeric_limits<float>::max();
	float minZ = numeric_limits<float>::max();

	float maxX = numeric_limits<float>::min();
	float maxY = numeric_limits<float>::min();
	float maxZ = numeric_limits<float>::min();

	FILE * file = fopen((const char*)fileName, "r");
	if( file == NULL ){
	    printf("Impossible to open the file !\n");
	    return;
	}

	while( 1 ){

		char lineHeader[128];

		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);

		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if ( strcmp( lineHeader, "v" ) == 0 ){

			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);

			if (vertex.x < minX)
				minX = vertex.x;
			if (vertex.y < minY)
				minY = vertex.y;
			if (vertex.z < minZ)
				minZ = vertex.z;

			if (vertex.x > maxX)
				maxX = vertex.x;
			if (vertex.y > maxY)
				maxY = vertex.y;
			if (vertex.z > maxZ)
				maxZ = vertex.z;

		}else if ( strcmp( lineHeader, "vt" ) == 0 ){

			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			temp_uvs.push_back(uv);

		}else if ( strcmp( lineHeader, "vn" ) == 0 ){

			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);

		}else if ( strcmp( lineHeader, "f" ) == 0 ){

			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

			// int matches = fscanf(file, "%d//%d//%d %d//%d//%d %d//%d//%d\n", &vertexIndex[0], &normalIndex[0], &uvIndex[0], &vertexIndex[1], &normalIndex[1], &uvIndex[1], &vertexIndex[2], &normalIndex[2], &uvIndex[2]);
			// if (matches != 9){

			int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
			if (matches != 6){

				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return;

			}

		    vertexIndices.push_back(vertexIndex[0]);
		    vertexIndices.push_back(vertexIndex[1]);
		    vertexIndices.push_back(vertexIndex[2]);

		    normalIndices.push_back(normalIndex[0]);
		    normalIndices.push_back(normalIndex[1]);
		    normalIndices.push_back(normalIndex[2]);

		    if (matches == 9){

			    uvIndices.push_back(uvIndex[0]);
			    uvIndices.push_back(uvIndex[1]);
			    uvIndices.push_back(uvIndex[2]);

		    }

	    }

	}

	// Vertices

    for(unsigned int i=0; i<vertexIndices.size(); i++){

		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[vertexIndex-1];
		_vertices.push_back(vertex);

	}

	// UVs

	for(unsigned int i=0; i<uvIndices.size(); i++){

		unsigned int uvIndex = uvIndices[i];
		glm::vec2 uv = glm::vec2(temp_uvs[uvIndex-1]);
		_uvs.push_back(uv);

	}

	for(unsigned int i=0; i<vertexIndices.size(); i++){

		unsigned int uvIndex = vertexIndices[i];
		glm::vec2 uv = glm::vec2((temp_vertices[uvIndex-1].x - minX)/(maxX-minX), (temp_vertices[uvIndex-1].z - minZ)/(maxZ-minZ));
		_uvsPlanar.push_back(uv);

	}

	glm::vec3 center = glm::vec3((maxX+minX)/2, (maxY+minY)/2, (maxZ+minZ)/2);

	for(unsigned int i=0; i<vertexIndices.size(); i++){

		unsigned int uvIndex = vertexIndices[i];

		glm::vec3 n = glm::normalize(temp_vertices[uvIndex-1] - center);
		float u = atan2(n.x, n.z) / (2*PI) + 0.5;
		float v = 0.5 - asin(n.y) / (PI);
		glm::vec2 uv = glm::vec2(v, u);
		_uvsSpherical.push_back(uv);

	}

	// normals

    for(unsigned int i=0; i<normalIndices.size(); i++){

		unsigned int normalIndex = normalIndices[i];
		glm::vec3 normal = temp_normals[normalIndex-1];
		_normals.push_back(normal);

	}

	initOpenGL();

}

// Protected

void MeshModel::initOpenGL(){

	// _textureID = loadBMP("/home/evgeny/computergraphics2018-sergey-evgeny/Data/Textures/cowFur.bmp");

	// Should be replaced

	_verticesSize = _vertices.size();

	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;

	glGenVertexArrays(1, &_VertexArrayID);
	glBindVertexArray(_VertexArrayID);

	// Vertices

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(glm::vec3), &_vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// UVs

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, _uvs.size() * sizeof(glm::vec2), &_uvs[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Normals

	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, _normals.size() * sizeof(glm::vec3), &_normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// UVs planar

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, _uvsPlanar.size() * sizeof(glm::vec2), &_uvsPlanar[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// UVs spherical

	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, _uvsSpherical.size() * sizeof(glm::vec2), &_uvsSpherical[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);

}

// Constructors

MeshModel::MeshModel(){
	_obj.resetObjParameters();
	_obj.resetWorldParameters();
}

MeshModel::MeshModel(const char* fileName): MeshModel(){
	loadOBJ(fileName);
}

MeshModel::MeshModel(float xMax, float xMin, float yMax, float yMin, float zMax, float zMin): MeshModel(){

	_primitive = true;

	// Points

	glm::vec3 p1 = glm::vec3(xMax, yMin, zMax);
	glm::vec3 p2 = glm::vec3(xMax, yMax, zMax);
	glm::vec3 p3 = glm::vec3(xMin, yMax, zMax);
	glm::vec3 p4 = glm::vec3(xMin, yMin, zMax);

	glm::vec3 p5 = glm::vec3(xMax, yMin, zMin);
	glm::vec3 p6 = glm::vec3(xMax, yMax, zMin);
	glm::vec3 p7 = glm::vec3(xMin, yMax, zMin);
	glm::vec3 p8 = glm::vec3(xMin, yMin, zMin);

	// Vertices

	_vertices.push_back(p1);
	_vertices.push_back(p2);
	_vertices.push_back(p3);

	_vertices.push_back(p1);
	_vertices.push_back(p3);
	_vertices.push_back(p4);

	_vertices.push_back(p7);
	_vertices.push_back(p6);
	_vertices.push_back(p5);

	_vertices.push_back(p8);
	_vertices.push_back(p7);
	_vertices.push_back(p5);

	_vertices.push_back(p2);
	_vertices.push_back(p7);
	_vertices.push_back(p3);

	_vertices.push_back(p2);
	_vertices.push_back(p6);
	_vertices.push_back(p7);

	_vertices.push_back(p1);
	_vertices.push_back(p8);
	_vertices.push_back(p5);

	_vertices.push_back(p1);
	_vertices.push_back(p4);
	_vertices.push_back(p8);

	_vertices.push_back(p2);
	_vertices.push_back(p5);
	_vertices.push_back(p6);

	_vertices.push_back(p2);
	_vertices.push_back(p1);
	_vertices.push_back(p5);

	_vertices.push_back(p4);
	_vertices.push_back(p3);
	_vertices.push_back(p7);

	_vertices.push_back(p4);
	_vertices.push_back(p7);
	_vertices.push_back(p8);

	// Normals

	_normals.push_back(p1);
	_normals.push_back(p2);
	_normals.push_back(p3);

	_normals.push_back(p1);
	_normals.push_back(p3);
	_normals.push_back(p4);

	_normals.push_back(p7);
	_normals.push_back(p6);
	_normals.push_back(p5);

	_normals.push_back(p8);
	_normals.push_back(p7);
	_normals.push_back(p5);

	_normals.push_back(p2);
	_normals.push_back(p7);
	_normals.push_back(p3);

	_normals.push_back(p2);
	_normals.push_back(p6);
	_normals.push_back(p7);

	_normals.push_back(p1);
	_normals.push_back(p8);
	_normals.push_back(p5);

	_normals.push_back(p1);
	_normals.push_back(p4);
	_normals.push_back(p8);

	_normals.push_back(p2);
	_normals.push_back(p5);
	_normals.push_back(p6);

	_normals.push_back(p2);
	_normals.push_back(p1);
	_normals.push_back(p5);

	_normals.push_back(p4);
	_normals.push_back(p3);
	_normals.push_back(p7);

	_normals.push_back(p4);
	_normals.push_back(p7);
	_normals.push_back(p8);

	// UVs

	for (int i = 0; i < _vertices.size()/3; i++){
		if (i % 2 == 1){
			_uvs.push_back(glm::vec2(0.0, 1.0));
			_uvs.push_back(glm::vec2(0.0, 0.0));
			_uvs.push_back(glm::vec2(1.0, 0.0));
		}else{
			_uvs.push_back(glm::vec2(0.0, 1.0));
			_uvs.push_back(glm::vec2(1.0, 1.0));
			_uvs.push_back(glm::vec2(1.0, 0.0));
		}
	}

	// UVs planar

	for(unsigned int i=0; i<_vertices.size(); i++){

		glm::vec2 uv = glm::vec2(_vertices[i].x - 0.5, _vertices[i].z - 0.5);
		_uvsPlanar.push_back(uv);

	}

	// UVs spherical

	for(unsigned int i=0; i<_vertices.size(); i++){

		glm::vec3 n = glm::normalize(_vertices[i]);
		float u = atan2(n.x, n.z) / (2*PI) + 0.5;
		float v = 0.5 - asin(n.y) / (PI);
		glm::vec2 uv = glm::vec2(v, u);
		_uvsSpherical.push_back(uv);

	}

	initOpenGL();

}

// Object

void MeshModel::getObjParameters(Object &obj, glm::vec4 &modelColorAmbient, glm::vec4 &modelColorDiffuse, glm::vec4 &modelColorSpecular){
	obj = _obj;
	modelColorAmbient = _modelColorAmbient;
	modelColorDiffuse = _modelColorDiffuse;
	modelColorSpecular = _modelColorSpecular;
}

void MeshModel::setObjParameters(Object obj, glm::vec4 modelColorAmbient, glm::vec4 modelColorDiffuse, glm::vec4 modelColorSpecular){
	_obj = obj;
	_modelColorAmbient = modelColorAmbient;
	_modelColorDiffuse = modelColorDiffuse;
	_modelColorSpecular = modelColorSpecular;
}

glm::mat4x4 MeshModel::getObjTransform(){

	return _obj.getRotationMatrix(2) * _obj.getMoveMatrix(2) * _obj.getRotationMatrix(1) * _obj.getScaleMatrix();

}

glm::vec3 MeshModel::getPosition(){

	return glm::vec3(_obj.getMoveMatrix(2) * _obj.getRotationMatrix(2) * _obj.getMoveMatrix(1) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

}

void MeshModel::ProcessMouseMovement(float xoffset, float yoffset, int mode){

    xoffset *= _rotationSensitivity;
    yoffset *= _rotationSensitivity;

	if (mode == 0){

	    _obj._zeta += xoffset;
	    _obj._phi += yoffset;

	}else if (mode == 1){

	    _obj._zetaWorld += xoffset;
	    _obj._phiWorld += yoffset;

	}

}

void MeshModel::ProcessKeyboard(int key, float deltaTime){

	switch (key){
		case 0:
			_obj._xPositionWorld += deltaTime*_moveSensitivity;
		break;
		case 1:
			_obj._xPositionWorld -= deltaTime*_moveSensitivity;
		break;
		case 2:
			_obj._zPositionWorld += deltaTime*_moveSensitivity;
		break;
		case 3:
			_obj._zPositionWorld -= deltaTime*_moveSensitivity;
		break;
		case 4:
			_obj._yPositionWorld += deltaTime*_moveSensitivity;
		break;
		case 5:
			_obj._yPositionWorld -= deltaTime*_moveSensitivity;
		break;

	}

}
