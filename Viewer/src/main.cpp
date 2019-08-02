// ImGui - standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (Glad is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, GL3W, etc.)

//----------------------------------------------------------------------------------------- Includes

#include <imgui/imgui.h>
#include <stdio.h>

#include <glad/glad.h>    // This example is using glad to access OpenGL functions. You may freely use any other OpenGL loader such as: glew, gl3w, glLoadGen, etc.
#include <GLFW/glfw3.h>

// Handling imgui+glfw
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Scene, rendering and mesh loading
#include "Renderer.h"
#include "Scene.h"
#include "ImguiMenus.h"

#include "InitShader.h"

#include <unistd.h>
#include <iostream>

using namespace std;

//----------------------------------------------------------------------------------------- Functions Header

// Setups the internal state of glfw, and intializing glad.
GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name);

// Callback for the error state of glfw
static void GlfwErrorCallback(int error, const char* description);

// Setup's the internal state of imgui.
ImGuiIO& SetupDearImgui(GLFWwindow* window);

// Takes care of all the opengl and glfw backend for rendering a new frame.
void StartFrame();

// Renders imgui. Takes care of screen resize, and finally renders the scene
void RenderFrame(GLFWwindow* window, Renderer* renderer);

// Cleanup routines of all the systems used here.
void Cleanup(GLFWwindow* window);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

//----------------------------------------------------------------------------------------- Variables

int width = 1866, height = 1056;
// int w = 1280, h = 720;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool firstMouse = true;

float lastX = width / 2.0f;
float lastY = height / 2.0f;

Renderer* renderer;
Scene* scene;

bool keyPressedLC;
bool keyPressedLS;
bool keyPressedLA;

bool keyPressedRC;
bool keyPressedRS;
bool keyPressedRA;

//----------------------------------------------------------------------------------------- Main

int main(int argc, char **argv)
{

	GLFWwindow* window = SetupGlfwWindow(width, height, "Mesh Viewer");
	if (!window)
		return 1;

    // Setup Dear ImGui binding
	ImGuiIO& io = SetupDearImgui(window);

	// Setup renderer and scene
	renderer = new Renderer(width, height);
	scene = new Scene(renderer);

	// Check if OBJ file entered
	if (argc == 2){
		scene->LoadOBJModel(argv[1]);
	}

	glfwSetCursorPosCallback(window, mouse_callback);

    // Main loop - the famous "Game Loop" in video games :)
    while (!glfwWindowShouldClose(window))
    {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

		processInput(window);


        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
	    glfwPollEvents();

		// draw scene here
		scene->Draw(window);

        // Start the ImGui frame
		StartFrame();

		// imgui stuff here
		DrawImguiMenus(io, scene);

        // Rendering + user rendering - finishing the ImGui frame
		// go to function implementation to add your rendering calls.
		RenderFrame(window, renderer);// --> go to line 137

    }

    // Cleanup
	Cleanup(window);

    return 0;

}

//------------------------------------------------------------------------------ Functions implementation

//---------------------------------------------------------------- Window (GLFW)

// Setups the internal state of glfw, and intializing glad.
GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name)
{

	glfwSetErrorCallback(GlfwErrorCallback);

	if (!glfwInit())
		return NULL;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	#if __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	GLFWwindow* window = glfwCreateWindow(w, h, window_name, NULL, NULL);

	glfwMakeContextCurrent(window);

	// Enable vsync
	// very importent!! initialization of glad
	// https://stackoverflow.com/questions/48582444/imgui-with-the-glad-opengl-loader-throws-segmentation-fault-core-dumped
	glfwSwapInterval(1);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	return window;

}

// Callback for the error state of glfw
static void GlfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

//---------------------------------------------------------------- Menus (Imgui)

// Setup's the internal state of imgui.
ImGuiIO& SetupDearImgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);
	return io;
}

//---------------------------------------------------------------- ???

// Takes care of all the opengl and glfw backend for rendering a new frame.
void StartFrame()
{

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

}

//---------------------------------------------------------------- Refresh view each frame

// Renders imgui. Takes care of screen resize, and finally renders the scene
void RenderFrame(GLFWwindow* window, Renderer* renderer)
{

	// creates ImGui's vertex buffers and textures
	ImGui::Render();

	// getting current frame buffer size. Dont get confused with glfwWindowSize!!!
	// frame buffer is in pixels, screen size in different units for different systems.
	int displayW, displayH;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &displayW, &displayH);

	// Telling opengl to resize the framebuffer
	renderer->_width = displayW;
	renderer->_height = displayH;
	glViewport(0, 0, displayW, displayH);
	
	// Actual rendering of ImGui. ImGui only creates buffers and textures, 
	// which are sent to opengl for the actual rendering.
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(window);

}

//---------------------------------------------------------------- Close window

void Cleanup(GLFWwindow* window)
{

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

}

//---------------------------------------------------------------- Mous & KeyBoard control

void processInput(GLFWwindow *window)
{

	// Camera rotation

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
    	keyPressedLC = true;
    }else{
    	keyPressedLC = false;
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
    	keyPressedLS = true;
    }else{
    	keyPressedLS = false;
    }

	// Camera displacement

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS){
    	keyPressedLA = true;
    }else{
    	keyPressedLA = false;
    }

    if (keyPressedLA && glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS){
		scene->ProcessKeyboard(0, deltaTime, 0);
    }

    if (keyPressedLA && glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS){
		scene->ProcessKeyboard(1, deltaTime, 0);
    }

    if (keyPressedLA && glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS){
		scene->ProcessKeyboard(2, deltaTime, 0);
    }

    if (keyPressedLA && glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS){
		scene->ProcessKeyboard(3, deltaTime, 0);
    }

    if (keyPressedLA && glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS){
		scene->ProcessKeyboard(4, deltaTime, 0);
    }

    if (keyPressedLA && glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS){
		scene->ProcessKeyboard(5, deltaTime, 0);
    }

	// Object rotation

    if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS){
    	keyPressedRC = true;
    }else{
    	keyPressedRC = false;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS){
    	keyPressedRS = true;
    }else{
    	keyPressedRS = false;
    }

	// Object displacement

    if (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS){
    	keyPressedRA = true;
    }else{
    	keyPressedRA = false;
    }

    if (keyPressedRA && glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS){
		scene->ProcessKeyboard(0, deltaTime, 1);
    }

    if (keyPressedRA && glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS){
		scene->ProcessKeyboard(1, deltaTime, 1);
    }

    if (keyPressedRA && glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS){
		scene->ProcessKeyboard(2, deltaTime, 1);
    }

    if (keyPressedRA && glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS){
		scene->ProcessKeyboard(3, deltaTime, 1);
    }

    if (keyPressedRA && glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS){
		scene->ProcessKeyboard(4, deltaTime, 1);
    }

    if (keyPressedRA && glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS){
		scene->ProcessKeyboard(5, deltaTime, 1);
    }

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    if (keyPressedLC){
		scene->ProcessMouseMovement(xoffset, yoffset, 0, 0);
    }else if (keyPressedLS){
		scene->ProcessMouseMovement(xoffset, yoffset, 1, 0);
    }

    if (keyPressedRC){
		scene->ProcessMouseMovement(xoffset, yoffset, 0, 1);
    }else if (keyPressedRS){
		scene->ProcessMouseMovement(xoffset, yoffset, 1, 1);
    }

}
