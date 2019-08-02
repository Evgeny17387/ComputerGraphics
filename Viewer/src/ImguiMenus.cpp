#pragma once

#include "ImguiMenus.h"
#include <stdio.h>
#include <stdlib.h>
#include <nfd.h>
#include <iostream>
#include "MeshModel.h"

using namespace std;

// Model

Object objModel = Object();

bool loadPrimitive = false;
bool primitiveLoaded = false;

int activeModel = -1;

float moveResolution = 0.5;

glm::vec4 modelColorAmbient;
glm::vec4 modelColorDiffuse;
glm::vec4 modelColorSpecular;

bool drawNormals;

// Camera

Object objCamera = Object();

int activeCamera = 0;

int cameraCounter = 1;

int trackObject;

bool renderCamera;

// Projection

int projectionType;

float near1;
float far1;
int fov;

float maxProjection = 100;

// Light

int lightCounter = 0;

int activeLight = -1;

Object objLight = Object();

glm::vec4 lightColor;

int sourceType;

int shiness;

// Scene

int shadow;

bool showFloor;

int floorTexture;

bool environment;

glm::vec4 ambientLight;

bool drawAxis;

int postProcessing;

bool fog;
int texture;

float maxFog = 1;
float fogFactor;

bool depth;

bool aliasing;

bool explode;

bool bloom;
float exposure;
float maxExposure = 1.0;
float offsetX;
float offsetY;
float maxOffset = 0.03;

glm::vec4 floorColorAmbient;
glm::vec4 floorColorDiffuse;
glm::vec4 floorColorSpecular;

// Other

bool showDemoWindow = false;
bool showAnotherWindow = false;
bool showFile = false;

void DrawImguiMenus(ImGuiIO& io, Scene* scene)
{

	ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Once);
	// 1. Show a simple window.
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
	{

		ImGui::Begin("Test Menu", 0, ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		// Object

		if (ImGui::CollapsingHeader("Object")){

			if (ImGui::Button("Load Model")){

				nfdchar_t *outPath = NULL;
				nfdresult_t result = NFD_OpenDialog("obj;png,jpg", NULL, &outPath);
				if (result == NFD_OKAY) {

					scene->LoadOBJModel(outPath);
					free(outPath);

				}
				else if (result == NFD_CANCEL) {
				}
				else {
				}

			}

			ImGui::Checkbox("Load Primitive", &loadPrimitive);
			if (loadPrimitive && !primitiveLoaded){
				primitiveLoaded = true;
				scene->loadPrimitive();
			}else if (!loadPrimitive && primitiveLoaded){
				primitiveLoaded = false;
				scene->unLoadPrimitive();
			}

			ImGui::Text("Active");
			ImGui::InputInt("Model #", &activeModel, 1);

			scene->getModelParameters(objModel, activeModel, modelColorAmbient, modelColorDiffuse, modelColorSpecular, texture, drawNormals);

			ImGui::Checkbox("Draw Normals", &drawNormals);

			ImGui::Text("Material");

			ImGui::ColorEdit3("Ambient Color", (float*)&modelColorAmbient);
			ImGui::ColorEdit3("Diffuse Color", (float*)&modelColorDiffuse);
			ImGui::ColorEdit3("Specular Color", (float*)&modelColorSpecular);

			ImGui::Text("Texture1");

	        ImGui::RadioButton("None1", &texture, 0);
	        ImGui::SameLine();
	        ImGui::RadioButton("Self1", &texture, 1);
	        ImGui::SameLine();
	        ImGui::RadioButton("Planar1", &texture, 2);
	        ImGui::SameLine();
	        ImGui::RadioButton("Spherical1", &texture, 3);
	        ImGui::SameLine();
	        ImGui::RadioButton("Reflection1", &texture, 4);

			if (ImGui::Button("Load1")){

				nfdchar_t *outPath = NULL;
				nfdresult_t result = NFD_OpenDialog("jpg", NULL, &outPath);
				if (result == NFD_OKAY) {

					scene->LoadTexture(outPath, activeModel);
					free(outPath);

				}
				else if (result == NFD_CANCEL) {
				}
				else {
				}

			}

			ImGui::SliderFloat("Scale", &objModel._zoom, 0.0f, 2.0f);

			if (ImGui::Button("Reset Model")){
				objModel.resetObjParameters();
				objModel.resetWorldParameters();
			}

			scene->setModelParameters(objModel, activeModel, modelColorAmbient, modelColorDiffuse, modelColorSpecular, texture, drawNormals);

    	}

		// Camera

    	if (ImGui::CollapsingHeader("Camera")){

			if (ImGui::Button("Add Camera")){
				cameraCounter++;
				scene->addCamera();
			}

			ImGui::SameLine();
			ImGui::Text("# of Cameras = %d", cameraCounter);

			ImGui::Text("Active");
			ImGui::InputInt("Camera #", &activeCamera, 1);

			// scene->setCameraCoordinates(eyeX, eyeY, eyeZ, atX, atY, atZ, upX, upY, upZ, activeCamera);

			scene->getCameraParameters(objCamera, activeCamera, renderCamera, trackObject);

			ImGui::Text("Track Object");
			ImGui::InputInt("Object #", &trackObject, 1);

			ImGui::Checkbox("Render Camera", &renderCamera);

			if (ImGui::Button("Reset Camera")){

				objCamera.resetWorldParameters();
				objCamera.resetObjParameters();

				objCamera._yPositionWorld = 2.0;
				objCamera._zPositionWorld = 5.0;
				objCamera._xPositionWorld = 5.0;

				objCamera._zPosition = -2.0;

				objCamera._phi = -10.0;
				objCamera._zeta = -40.0;
			}

			scene->setCameraParameters(objCamera, activeCamera, renderCamera, trackObject);

			// Projection

			ImGui::Text("Projection");

			scene->updateProjection(fov, near1, far1, projectionType, activeCamera);

	        ImGui::RadioButton("Ortho", &projectionType, 0);
	        ImGui::SameLine();
	        ImGui::RadioButton("Perspective", &projectionType, 1);

			ImGui::SliderInt("FOV", &fov, 0, 180);

			ImGui::SliderFloat("Near", &near1, -maxProjection, maxProjection);
			ImGui::SliderFloat("Far", &far1, -maxProjection, maxProjection);

			if (ImGui::Button("Reset Projection")){
				switch (projectionType){
					case 0:
						near1 = -maxProjection;
						far1 = maxProjection;
						break;
					case 1:
						near1 = 0.01;
						far1 = maxProjection;
						fov = 45.0;
						break;
				}
			}
			
			scene->setProjection(fov, near1, far1, projectionType, activeCamera);

		}

		// Light

		if (ImGui::CollapsingHeader("Light")){

			if (ImGui::Button("Add Light")){
				lightCounter++;
				scene->addLight(sourceType);
			}

			ImGui::SameLine();
			ImGui::Text("# of Lights = %d", lightCounter);

			ImGui::Text("Active");
			ImGui::InputInt("Light #", &activeLight, 1);

			scene->getLightParameters(objLight, activeLight, lightColor, sourceType, shiness);

	        ImGui::RadioButton("Point Source", &sourceType, 0);
	        ImGui::SameLine();
	        ImGui::RadioButton("parallel Source", &sourceType, 1);

			ImGui::ColorEdit3("Light Color", (float*)&lightColor);

			ImGui::SliderInt("Shiness", &shiness, 0, 100);

			ImGui::Text("Object");

			if (ImGui::Button("X4+"))
				objLight._xPosition+=moveResolution;
			ImGui::SameLine();
			if (ImGui::Button("X4-"))
				objLight._xPosition-=moveResolution;

			if (ImGui::Button("Y4+"))
				objLight._yPosition+=moveResolution;
			ImGui::SameLine();
			if (ImGui::Button("Y4-"))
				objLight._yPosition-=moveResolution;

			if (ImGui::Button("Z4+"))
				objLight._zPosition+=moveResolution;
			ImGui::SameLine();
			if (ImGui::Button("Z4-"))
				objLight._zPosition-=moveResolution;

			if (ImGui::Button("Reset4"))
				objLight.resetObjParameters();

			ImGui::Text("World");

			ImGui::SliderInt("Theta5", &objLight._thetaWorld, -180, 180);
			ImGui::SliderInt("Phi5", &objLight._phiWorld, -180, 180);
			ImGui::SliderInt("Zeta5", &objLight._zetaWorld, -180, 180);

			if (ImGui::Button("Reset5"))
				objLight.resetWorldParameters();

			scene->setLightParameters(objLight, activeLight, lightColor, shiness);

		}

		// Scene

		if (ImGui::CollapsingHeader("Scene")){

			scene->updateScene(shadow, showFloor, floorTexture, environment, floorColorAmbient, floorColorDiffuse, floorColorSpecular, ambientLight, drawAxis, postProcessing, fog, fogFactor, depth, aliasing, explode, bloom, exposure, offsetX, offsetY);

			ImGui::Checkbox("Environment", &environment);

			ImGui::ColorEdit3("Ambient Light", (float*)&ambientLight);

			ImGui::Text("Floor");

			ImGui::Checkbox("Show Floor", &showFloor);

			ImGui::ColorEdit3("Ambient", (float*)&floorColorAmbient);
			ImGui::ColorEdit3("Diffuse", (float*)&floorColorDiffuse);
			ImGui::ColorEdit3("Specular", (float*)&floorColorSpecular);

			ImGui::Text("Texture2");

	        ImGui::RadioButton("None2", &floorTexture, 0);
	        ImGui::SameLine();
	        ImGui::RadioButton("Self2", &floorTexture, 1);
	        ImGui::SameLine();
	        ImGui::RadioButton("Reflection2", &floorTexture, 4);

			ImGui::Checkbox("Draw Axis", &drawAxis);

			ImGui::Text("Special Effects");

	        ImGui::Checkbox("Fog", &fog);
	        ImGui::SameLine();
			ImGui::SliderFloat("Fog Factor", &fogFactor, 0, maxFog);

	        ImGui::Checkbox("Visualize Depth", &depth);

	        ImGui::Checkbox("Anti-aliasing", &aliasing);

	        ImGui::Checkbox("Explode", &explode);

	        ImGui::Checkbox("Bloom", &bloom);
	        ImGui::SameLine();
			ImGui::SliderFloat("Exposurer", &exposure, 0, maxExposure);
			// ImGui::SliderFloat("offsetX", &offsetX, -maxOffset, maxOffset);
			// ImGui::SliderFloat("offsetY", &offsetY, -maxOffset, maxOffset);

			ImGui::Text("Shadow");

	        ImGui::RadioButton("None", &shadow, 0);
	        ImGui::SameLine();
	        ImGui::RadioButton("Parallel Light", &shadow, 1);
	        ImGui::SameLine();
	        ImGui::RadioButton("Point Light", &shadow, 2);

			ImGui::Text("Post-Processing");

	        ImGui::RadioButton("Normal", &postProcessing, 0);
	        ImGui::SameLine();
	        ImGui::RadioButton("Inverse", &postProcessing, 1);
	        ImGui::SameLine();
	        ImGui::RadioButton("Gray Scale", &postProcessing, 2);
	        ImGui::SameLine();
	        ImGui::RadioButton("Sharpen", &postProcessing, 3);
	        ImGui::SameLine();
	        ImGui::RadioButton("Edge-Detection", &postProcessing, 4);
	        ImGui::SameLine();
	        ImGui::RadioButton("Blur", &postProcessing, 5);

			scene->setScene(shadow, showFloor, floorTexture, environment, floorColorAmbient, floorColorDiffuse, floorColorSpecular, ambientLight, drawAxis, postProcessing, fog, fogFactor, depth, aliasing, explode, bloom, exposure, offsetX, offsetY);

		}

		// Others

    	// if (ImGui::CollapsingHeader("Other")){
    	if (false){

			// Window size

			ImVec2 last_tooltip_size = ImGui::GetWindowSize();

			int w = last_tooltip_size.x;
			int h = last_tooltip_size.y;

			ImGui::Text("Window Scalling");
			ImGui::InputInt("Width", &w, 1);
			ImGui::InputInt("Height", &h, 1);

			ImGui::Checkbox("Demo Window", &showDemoWindow);      // Edit bools storing our windows open/close state

			ImGui::Checkbox("Another Window", &showAnotherWindow);

			static int counter = 0;
			if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

		}

		ImGui::End();

	}

	// 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
	if (showAnotherWindow)
	{
		int val[2]; val[0] = io.MousePos.x; val[1] = io.MousePos.y;
		ImGui::Begin("Another Window", &showAnotherWindow);
		ImGui::InputInt2("(x,y)", val, 3);
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			showAnotherWindow = false;
		ImGui::End();
	}

	// 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
	if (showDemoWindow)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
		ImGui::ShowDemoWindow(&showDemoWindow);
	}

	// Demonstrate creating a fullscreen menu bar and populating it.
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoFocusOnAppearing;
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open", "CTRL+O"))
				{
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
				if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
				ImGui::Separator();
				if (ImGui::MenuItem("Cut", "CTRL+X")) {}
				if (ImGui::MenuItem("Copy", "CTRL+C")) {}
				if (ImGui::MenuItem("Paste", "CTRL+V")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("Show Demo Menu")) { showDemoWindow = true; }
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}
}
