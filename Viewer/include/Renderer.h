#pragma once

#include <vector>
#include <imgui/imgui.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "Light.h"

using namespace std;

class Renderer{

private:

    // Shaders

	GLuint _programPhong;
	GLuint _programNormals;
	GLuint _programExplode;
	GLuint _programBlur;
	GLuint _programBloomfinal;
	GLuint _programScreen;
	GLuint _programSkyBox;
	GLuint _program;
	GLuint _programReflection;
	GLuint _programShadowDepth;
	GLuint _programShadowScreen;
	GLuint _programShadow;
	GLuint _programShadowPointSource;
	GLuint _programShadowDepthPointSource;

    const unsigned int _SHADOW_WIDTH = 1024, _SHADOW_HEIGHT = 1024;

    // Background vertices

	GLuint _screenID;

    GLuint _cubeVAO;

    float _near_plane = 1.0;
    float _far_plane = 25.0;

public:

	// Screen

	GLuint _framebuffer;
	GLuint _colorBuffer;

	// Antialiasing

	GLuint _framebufferAntialiasing;
    GLuint _colorBufferAntialiasing;

	// Bloom

	GLuint _framebufferBloom;
    GLuint _colorBuffersBloom[2];

    GLuint _pingpongFBO[2];
    GLuint _pingpongColorbuffers[2];

    // Shadows

    GLuint _depthMapFBO;
    GLuint _depthMap;

    GLuint _depthMapFBOPointsSource;
    GLuint _depthMapPointsSource;

    // Background vertices

	GLuint _skyboxVAO;
	GLuint _cubemapTexture;

	GLuint _floorID;
	GLuint _floorTextureID;

	GLuint _AxisID;

	int _postProcessing = 0;

	glm::mat4x4 _mTransform;
	glm::mat4x4 _vTransform;
	glm::mat4x4 _projection;

	int _width, _height;

	glm::vec4 _lightSourceColor;
	glm::vec3 _lightPosition;
	glm::vec3 _lightDirection;
	float _lightSourceShiness;

	glm::vec3 _cameraPosition;

	bool _fog = false;
	float _fogFactor = 0.2;

	bool _depth = false;

	bool _aliasing = false;

	bool _explode = false;

	bool _bloom = false;
	float _exposure = 0.1;
	float _offsetX = -0.01;
	float _offsetY = 0.0;

	glm::vec4 _ambientLight = glm::vec4(1.0, 1.0, 1.0, 1.0);

	vector<Light*> * _lights;

	glm::vec4 _modelColorAmbient;
	glm::vec4 _modelColorDiffuse;
	glm::vec4 _modelColorSpecular;

	glm::vec4 _floorColorAmbient = glm::vec4(1.0, 1.0, 1.0, 1.0);
	glm::vec4 _floorColorDiffuse = glm::vec4(1.0, 1.0, 1.0, 1.0);
	glm::vec4 _floorColorSpecular = glm::vec4(1.0, 1.0, 1.0, 1.0);

	bool _environment = false;

	int _floorTexture = 0;

	bool _showFloor = true;

	int _shadow = 2;

	glm::mat4 _lightSpaceMatrix;
	glm::vec3 _lightPos;

	// Initiation

	Renderer();
	Renderer(int width, int height);

	void initOpenGLRendering();

	// OpnGL running

	void DrawObject(int iteration, bool reflection, bool skyBox, bool explode, bool isAxis, bool drawNormals, GLFWwindow* window, GLuint VertexArrayID, int verticesSize, GLuint textureID, int texture, bool lightSource = false, glm::vec4 sourceColor = glm::vec4(0.0, 0.0, 0.0, 1.0));

	void DrawFrame();

	void PrepareFrame();

	void RefreshFrame();

};
