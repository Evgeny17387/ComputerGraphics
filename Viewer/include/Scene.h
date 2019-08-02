#pragma once

#include <string>
#include <vector>
#include "Renderer.h"
#include "MeshModel.h"
#include "Camera.h"
#include "Light.h"
#include "Object.h"

#include <glm/glm.hpp>

using namespace std;

class Scene {

private:

	vector<MeshModel*> models;
	vector<Camera*> cameras;
	Renderer *renderer;

	int _activeModel;
	int _activeLight;
	int _activeCamera = 0;

	bool _drawAxis = false;

public:

	vector<Light*> lights;

	Scene();
	Scene(Renderer *renderer);

	// Draw

	void Draw(GLFWwindow* window);

	// Model

	void loadPrimitive();
	void unLoadPrimitive();

	void LoadOBJModel(const char* fileName);
	void LoadTexture(const char* fileName, int activeModel);

	void getModelParameters(Object &obj, int activeModel, glm::vec4 &modelColorAmbient, glm::vec4 &modelColorDiffuse, glm::vec4 &modelColorSpecular, int &texture, bool &drawNormals);
	void setModelParameters(Object obj, int activeModel, glm::vec4 modelColorAmbient, glm::vec4 modelColorDiffuse, glm::vec4 modelColorSpecular, int texture, bool drawNormals);

	// Camera

	void addCamera();

	void getCameraParameters(Object &obj, int activeCamera, bool &renderCamera, int &trackObject);
	void setCameraParameters(Object obj, int activeCamera, bool renderCamera, int trackObject);

	void updateProjection(int &fov, float &near1, float &far1, int &projectionType, int activeCamera);
	void setProjection(int fov, float near1, float far1, int projectionType, int activeCamera);

	// Light

	void addLight(int sourceType);

	void getLightParameters(Object &obj, int &activeLight, glm::vec4 &color, int &sourceType, int &shiness);
	void setLightParameters(Object obj, int activeLight, glm::vec4 color, int shiness);

	// Scene

	void updateScene(int &shadow, bool &showFloor, int &floorTexture, bool &environment, glm::vec4 &floorColorAmbient, glm::vec4 &floorColorDiffuse, glm::vec4 &floorColorSpecular, glm::vec4 &ambientLight, bool &drawAxis, int &postProcessing, bool &fog, float &fogfactor, bool &depth, bool &aliasing, bool &explode, bool &bloom, float &exposure, float &OffsetX, float &OffsetY);
	void setScene(int shadow, bool showFloor, int floorTexture, bool environment, glm::vec4 floorColorAmbient, glm::vec4 floorColorDiffuse, glm::vec4 floorColorSpecular, glm::vec4 ambientLight, bool drawAxis, int postProcessing, bool fog, float fogfactor, bool depth, bool aliasing, bool explode, bool bloom, float exposure, float OffsetX, float OffsetY);

	// Control

	void ProcessMouseMovement(float xoffset, float yoffset, int mode, int obj);
	void ProcessKeyboard(int key, float deltaTime, int obj);

};
