#include <iostream>

#include "Scene.h"
#include "Utils.h"

using namespace std;

Scene::Scene(){
};

Scene::Scene(Renderer *renderer) : renderer(renderer) {

	Camera *camera = new Camera();

	cameras.push_back(camera);

	renderer->_lights = &(this->lights);

};

// Draw

void Scene::Draw(GLFWwindow* window){

	// Set camera

	Camera *camera = (Camera*) cameras[_activeCamera];

	camera->_width = renderer->_width;
	camera->_height = renderer->_height;

	renderer->_projection = camera->getProjection();

	if (camera->_trackObject >= 0 && camera->_trackObject < models.size()){
		renderer->_vTransform = camera->LookAt(((MeshModel*) models[camera->_trackObject])->getPosition());
	}else{
		renderer->_vTransform = glm::inverse(camera->getObjTransform());
	}

	renderer->_cameraPosition = camera->getPosition();

	// Prepare a frame

	renderer->PrepareFrame();

	int numberOfIteration = renderer->_shadow != 0 ? 2 : 1;

	for (int iteration = 0; iteration < numberOfIteration; iteration++){

		// Draw floor

		if (renderer->_showFloor){

			renderer->_mTransform = glm::mat4(1.0);
			renderer->_modelColorAmbient = renderer->_floorColorAmbient;
			renderer->_modelColorDiffuse = renderer->_floorColorDiffuse;
			renderer->_modelColorSpecular = renderer->_floorColorSpecular;
			renderer->DrawObject(iteration, true, false, false, false, false, window, renderer->_floorID, 6, renderer->_floorTextureID, renderer->_floorTexture);

		}

		// Draw axis

		if (_drawAxis){

			renderer->_mTransform = glm::mat4(1.0);

			renderer->DrawObject(iteration, false, false, false, true, true, window, renderer->_AxisID, 3, 0, 0);

		}

		// Draw models

		for (int i = 0; i < models.size(); i++){

			MeshModel* model = (MeshModel*) models[i];

			renderer->_mTransform = model->getObjTransform();

			renderer->_modelColorAmbient = model->_modelColorAmbient;
			renderer->_modelColorDiffuse = model->_modelColorDiffuse;
			renderer->_modelColorSpecular = model->_modelColorSpecular;

			renderer->DrawObject(iteration, true, false, true, false, model->_drawNormals, window, model->_VertexArrayID, model->_verticesSize, model->_textureID, model->_texture);

		}

		// Draw cameras

		for (int i = 0; i < cameras.size(); i++){

			Camera *camera = (Camera*) cameras[i];

			if (camera->_renderCamera){

				renderer->_mTransform = camera->getObjTransformScaled();

				renderer->_modelColorAmbient = camera->cameraModel->_modelColorAmbient;
				renderer->_modelColorDiffuse = camera->cameraModel->_modelColorDiffuse;
				renderer->_modelColorSpecular = camera->cameraModel->_modelColorSpecular;

				renderer->DrawObject(iteration, false, false, false, false, false, window, camera->cameraModel->_VertexArrayID, camera->cameraModel->_verticesSize, 0, 0);

			}

		}

		// Draw ligts

		for (int i = 0; i < lights.size(); i++){

			Light *light = (Light*) lights[i];

			if (light->_sourceType == 0){

				renderer->_mTransform = light->getObjTransform();

				renderer->_modelColorAmbient = light->lightModel->_modelColorAmbient;
				renderer->_modelColorDiffuse = light->lightModel->_modelColorDiffuse;
				renderer->_modelColorSpecular = light->lightModel->_modelColorSpecular;

				renderer->DrawObject(iteration, false, false, false, false, false, window, light->lightModel->_VertexArrayID, light->lightModel->_verticesSize, 0, 0, true, light->_color);

			}

		}

		// SkyBoxs

		if (renderer->_environment){
			renderer->DrawObject(iteration, false, true, false, false, false, window, renderer->_skyboxVAO, 36, renderer->_cubemapTexture, 1);
		}

		if (numberOfIteration == 2 && iteration == 0){
			renderer->RefreshFrame();
		}

	}

	renderer->DrawFrame();

}

// Model

void Scene::LoadOBJModel(const char* fileName){

	MeshModel *model = new MeshModel(fileName);
	models.push_back(model);

}

void Scene::LoadTexture(const char* fileName, int activeModel){

	if (activeModel < 0 || activeModel >= models.size())
		return;

	MeshModel* model = ((MeshModel*) models[activeModel]);

	model->_textureID = loadJPG(fileName);

}

void Scene::loadPrimitive(){

	MeshModel *model = new MeshModel(0.5, -0.5, 0.5, -0.5, 0.5, -0.5);
	models.push_back(model);

}

void Scene::unLoadPrimitive(){

	for (int i = 0; i < models.size(); i++){

		MeshModel* model = (MeshModel*) models[i];

		if (model->_primitive){
			
			models.erase(models.begin()+i, models.begin()+i+1);
			break;			

		}

	}

}

void Scene::getModelParameters(Object &obj, int activeModel, glm::vec4 &modelColorAmbient, glm::vec4 &modelColorDiffuse, glm::vec4 &modelColorSpecular, int &texture, bool &drawNormals){

	_activeModel = activeModel;

	if (activeModel < 0 || activeModel >= models.size())
		return;

	MeshModel* model = ((MeshModel*) models[activeModel]);

	model->getObjParameters(obj, modelColorAmbient, modelColorDiffuse, modelColorSpecular);
	texture = model->_texture;
	drawNormals = model->_drawNormals;

}

void Scene::setModelParameters(Object obj, int activeModel, glm::vec4 modelColorAmbient, glm::vec4 modelColorDiffuse, glm::vec4 modelColorSpecular, int texture, bool drawNormals){

	_activeModel = activeModel;

	if (activeModel < 0 || activeModel >= models.size())
		return;

	MeshModel* model = ((MeshModel*) models[activeModel]);

	model->setObjParameters(obj, modelColorAmbient, modelColorDiffuse, modelColorSpecular);

	if (texture == 2 || texture == 3)
		if (model->_textureID != 0)
			model->_texture = texture;

	if (texture == 1)
		if (model->_textureID != 0 && model->_uvs.size() > 0)
			model->_texture = texture;

	if (texture == 0)
		model->_texture = texture;

	if (texture == 4 && renderer->_environment)
		model->_texture = texture;

	model->_drawNormals = drawNormals;

}

// Camera

void Scene::addCamera(){

	Camera *camera = new Camera();
	cameras.push_back(camera);

}

void Scene::getCameraParameters(Object &obj, int activeCamera, bool &renderCamera, int &trackObject){

	if (activeCamera < 0 || activeCamera >= cameras.size())
		return;

	_activeCamera = activeCamera;

	((Camera*) cameras[activeCamera])->getObjParameters(obj, renderCamera);

	trackObject = ((Camera*) cameras[activeCamera])->_trackObject;

}

void Scene::setCameraParameters(Object obj, int activeCamera, bool renderCamera, int trackObject){

	if (activeCamera < 0 || activeCamera >= cameras.size())
		return;

	_activeCamera = activeCamera;

	((Camera*) cameras[activeCamera])->setObjParameters(obj, renderCamera);

	((Camera*) cameras[activeCamera])->_trackObject = trackObject;

}

void Scene::updateProjection(int &fov, float &near1, float &far1, int &projectionType, int activeCamera){

	if (activeCamera < 0 || activeCamera >= cameras.size())
		return;

	_activeCamera = activeCamera;

	Camera *camera = (Camera*) cameras[activeCamera];

	camera->updateProjection(fov, near1, far1, projectionType);

}

void Scene::setProjection(int fov, float near1, float far1, int projectionType, int activeCamera){

	if (activeCamera < 0 || activeCamera >= cameras.size())
		return;

	_activeCamera = activeCamera;

	Camera *camera = (Camera*) cameras[activeCamera];

	camera->setProjection(fov, near1, far1, projectionType);

}

// Light

void Scene::addLight(int sourceType){

	Light *light = new Light(sourceType);
	lights.push_back(light);

}

void Scene::getLightParameters(Object &obj, int &activeLight, glm::vec4 &color, int &sourceType, int &shiness){

	if (activeLight < 0 || activeLight >= lights.size())
		return;

	_activeLight = activeLight;

	Light* light = ((Light*) lights[activeLight]);

	light->getObjParameters(obj, color, sourceType);

	shiness = light->_shiness;

}

void Scene::setLightParameters(Object obj, int activeLight, glm::vec4 color, int shiness){

	if (activeLight < 0 || activeLight >= lights.size())
		return;

	_activeLight = activeLight;

	Light* light = ((Light*) lights[activeLight]);

	light->setObjParameters(obj, color);

	light->_shiness = shiness;

}

// Scene

void Scene::updateScene(int &shadow, bool &showFloor, int &floorTexture, bool &environment, glm::vec4 &floorColorAmbient, glm::vec4 &floorColorDiffuse, glm::vec4 &floorColorSpecular, glm::vec4 &ambientLight, bool &drawAxis, int &postProcessing, bool &fog, float &fogfactor, bool &depth, bool &aliasing, bool &explode, bool &bloom, float &exposure, float &OffsetX, float &OffsetY){

	drawAxis = _drawAxis;

	ambientLight = renderer->_ambientLight;

	postProcessing = renderer->_postProcessing;

	fog = renderer->_fog;
	fogfactor = renderer->_fogFactor;

	depth = renderer->_depth;

	aliasing = renderer->_aliasing;

	explode = renderer->_explode;

	bloom = renderer->_bloom;
	exposure = renderer->_exposure;
	OffsetX = renderer->_offsetX;
	OffsetY = renderer->_offsetY;

	floorColorAmbient = renderer->_floorColorAmbient;
	floorColorDiffuse = renderer->_floorColorDiffuse;
	floorColorSpecular = renderer->_floorColorSpecular;

	environment = renderer->_environment;

	floorTexture = renderer->_floorTexture;

	showFloor = renderer->_showFloor;

	shadow = renderer->_shadow;

}

void Scene::setScene(int shadow, bool showFloor, int floorTexture, bool environment, glm::vec4 floorColorAmbient, glm::vec4 floorColorDiffuse, glm::vec4 floorColorSpecular, glm::vec4 ambientLight, bool drawAxis, int postProcessing, bool fog, float fogfactor, bool depth, bool aliasing, bool explode, bool bloom, float exposure, float OffsetX, float OffsetY){

	_drawAxis = drawAxis;

	renderer->_ambientLight = ambientLight;

	renderer->_postProcessing = postProcessing;

	renderer->_fog = fog;
	renderer->_fogFactor = fogfactor;

	renderer->_depth = depth;

	renderer->_aliasing = aliasing;

	renderer->_explode = explode;

	renderer->_bloom = bloom;
	renderer->_exposure = exposure;
	renderer->_offsetX = OffsetX;
	renderer->_offsetY = OffsetY;

	renderer->_floorColorAmbient = floorColorAmbient;
	renderer->_floorColorDiffuse = floorColorDiffuse;
	renderer->_floorColorSpecular = floorColorSpecular;

	renderer->_environment = environment;

	if (renderer->_environment){
		renderer->_floorTexture = floorTexture;
	}else if (floorTexture != 4){
		renderer->_floorTexture = floorTexture;
	}

	renderer->_showFloor = showFloor;

	renderer->_shadow = shadow;

}

// Control

void Scene::ProcessMouseMovement(float xoffset, float yoffset, int mode, int obj){

	Camera *camera;
	MeshModel* model;

	switch (obj){
		case 0:
			camera = (Camera*) cameras[_activeCamera];
			camera->ProcessMouseMovement(xoffset, yoffset, mode);
		break;
		case 1:
			if (_activeModel < 0 || _activeModel >= models.size())
			return;
			model = ((MeshModel*) models[_activeModel]);
			model->ProcessMouseMovement(xoffset, yoffset, mode);
		break;
	}

}

void Scene::ProcessKeyboard(int key, float deltaTime, int obj){

	Camera *camera;
	MeshModel* model;

	switch (obj){
		case 0:
			camera = (Camera*) cameras[_activeCamera];
			camera->ProcessKeyboard(key, deltaTime);
		break;
		case 1:
			if (_activeModel < 0 || _activeModel >= models.size())
			return;
			model = ((MeshModel*) models[_activeModel]);
			model->ProcessKeyboard(key, deltaTime);
		break;
	}

}
