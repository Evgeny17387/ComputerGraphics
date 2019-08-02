#include <iostream>
#include <cstdint>
#include <string>
#include <math.h>

#include <imgui/imgui.h>
#include <glm/gtc/matrix_transform.hpp>

#include "ImguiMenus.h"
#include "Scene.h"
#include "Renderer.h"
#include "InitShader.h"
#include "Utils.h"

using namespace std;

//------------------------------------------------------------------------------ Initiation

Renderer::Renderer() : _width(1280), _height(720)
{
	Renderer(1280, 720);
}

Renderer::Renderer(int width, int height) : _width(width), _height(height)
{
	initOpenGLRendering();
}

void Renderer::initOpenGLRendering(){

    glEnable(GL_DEPTH_TEST);

    _floorTextureID = loadBMP("/home/evgeny17387/Desktop/ComputerGraphics/Data/Textures/floor.bmp");

	_programPhong = Shader(
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/vPhong.glsl",
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/fPhong.glsl"
                                );

    _programNormals = Shader(
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/vNormals.glsl",
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/fNormals.glsl",
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/gNormals.glsl"
                                );

    _programExplode = Shader(
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/vPhong.glsl",
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/fPhong.glsl",
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/gExplode.glsl"
                                );

    _programScreen = Shader(
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/vScreen.glsl",
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/fScreen.glsl"
                                );

    _programBlur = Shader(
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/vBlur.glsl",
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/fBlur.glsl"
                                );

    _programBloomfinal = Shader(
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/vBloomFinal.glsl",
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/fBloomFinal.glsl"
                                );

    _programSkyBox = Shader(
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/vSkyBox.glsl",
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/fSkyBox.glsl"
                                );

    _programReflection = Shader(
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/vReflection.glsl",
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/fReflection.glsl"
                                );

    _programShadowDepth = Shader(
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/vShadowDepth.glsl",
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/fShadowDepth.glsl"
                                );

    _programShadow = Shader(
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/vShadow.glsl",
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/fShadow.glsl"
                                );

    _programShadowScreen = Shader(
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/vShadowScreen.glsl",
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/fShadowScreen.glsl"
                                );

    _programShadowPointSource = Shader(
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/vShadowPointSource.glsl",
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/fShadowPointSource.glsl"
                                );

    _programShadowDepthPointSource = Shader(
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/vShadowDepthPointSource.glsl",
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/fShadowDepthPointSource.glsl",
                                    "/home/evgeny17387/Desktop/ComputerGraphics/Viewer/shaders/gShadowDepthPointSource.glsl"
                                );

    // Frame Buffer - Screen

    glGenFramebuffers(1, &_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

    glGenTextures(1, &_colorBuffer);
    glBindTexture(GL_TEXTURE_2D, _colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorBuffer, 0); // we only need a color buffer

    GLuint rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << endl;

    // Frame Buffer - Antialiasing

    glGenFramebuffers(1, &_framebufferAntialiasing);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebufferAntialiasing);

    glGenTextures(1, &_colorBufferAntialiasing);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, _colorBufferAntialiasing);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, _width, _height, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, _colorBufferAntialiasing, 0);

    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, _width, _height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    // Frame Buffer - Bloom

    glGenFramebuffers(1, &_framebufferBloom);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebufferBloom);

    glGenTextures(2, _colorBuffersBloom);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, _colorBuffersBloom[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _width, _height, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _colorBuffersBloom[i], 0);
    }

    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    glGenFramebuffers(2, _pingpongFBO);
    glGenTextures(2, _pingpongColorbuffers);

    for (unsigned int i = 0; i < 2; i++)
    {

        glBindFramebuffer(GL_FRAMEBUFFER, _pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, _pingpongColorbuffers[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _width, _height, 0, GL_RGB, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _pingpongColorbuffers[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;

    }

    // Frame Buffer - Shadow - Parallel Source

    glGenFramebuffers(1, &_depthMapFBO);
    glGenTextures(1, &_depthMap);
    glBindTexture(GL_TEXTURE_2D, _depthMap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _SHADOW_WIDTH, _SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Frame Buffer - Shadow - Point Source

    glGenFramebuffers(1, &_depthMapFBOPointsSource);
    glGenTextures(1, &_depthMapPointsSource);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _depthMapPointsSource);

    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, _SHADOW_WIDTH, _SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBOPointsSource);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthMapPointsSource, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Screen

    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    GLuint quadVBO;
    glGenVertexArrays(1, &_screenID);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(_screenID);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // Background floor

    float planeVertices[] = {
        -5.0f, 0.0f,-5.0f,  0.0, 1.0, 0.0,   0.0f, 2.0f,
        -5.0f, 0.0f, 5.0f,  0.0, 1.0, 0.0,   0.0f, 0.0f,
         5.0f, 0.0f, 5.0f,  0.0, 1.0, 0.0,   2.0f, 0.0f,

         5.0f, 0.0f,-5.0f,  0.0, 1.0, 0.0,   2.0f, 2.0f,
        -5.0f, 0.0f,-5.0f,  0.0, 1.0, 0.0,   0.0f, 2.0f,
         5.0f, 0.0f, 5.0f,  0.0, 1.0, 0.0,   2.0f, 0.0f
    };

    GLuint planeVBO;
    glGenVertexArrays(1, &_floorID);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(_floorID);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    // Axis

    float axisVertices[] = {
        // positions       // normals         // colors
        0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f
    };

    GLuint axisVBO;
    glGenVertexArrays(1, &_AxisID);
    glGenBuffers(1, &axisVBO);
    glBindVertexArray(_AxisID);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), &axisVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));

    // SkyBox

    float maxCoordinate = 100.0;

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    for(int i = 0; i < 6*6*3; i++)
        skyboxVertices[i] *= maxCoordinate;

    unsigned int skyboxVBO;
    glGenVertexArrays(1, &_skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(_skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    vector<std::string> faces
    {
        "/home/evgeny17387/Desktop/ComputerGraphics/Data/Environments/skybox/right.jpg",
        "/home/evgeny17387/Desktop/ComputerGraphics/Data/Environments/skybox/left.jpg",
        "/home/evgeny17387/Desktop/ComputerGraphics/Data/Environments/skybox/top.jpg",
        "/home/evgeny17387/Desktop/ComputerGraphics/Data/Environments/skybox/bottom.jpg",
        "/home/evgeny17387/Desktop/ComputerGraphics/Data/Environments/skybox/front.jpg",
        "/home/evgeny17387/Desktop/ComputerGraphics/Data/Environments/skybox/back.jpg"
    };
    unsigned int _cubemapTexture = loadCubemap(faces);

    // Cube for testing

    float verticesCube[] = {
        // back face
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
        // front face
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
        // left face
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
        // right face
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
         1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
        // bottom face
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
         1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
        // top face
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
         1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
         1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left 
    };

    GLuint cubeVBO;

    glGenVertexArrays(1, &_cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCube), verticesCube, GL_STATIC_DRAW);

    glBindVertexArray(_cubeVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

}

//------------------------------------------------------------------------------ OpenGL running

void Renderer::PrepareFrame(){

    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    glClearColor(0.0, 0.0, 0.0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, _framebufferAntialiasing);
    glClearColor(0.0, 0.0, 0.0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, _framebufferBloom);
    glClearColor(0.0, 0.0, 0.0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_shadow == 1){

        glViewport(0, 0, _SHADOW_WIDTH, _SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        //

        glUseProgram(_programShadowDepth);

        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, _near_plane, _far_plane);

        if (_lights->size()> 0){
            _lightDirection = ((Light*) (*_lights)[0])->getDirection();
            _lightSourceColor = ((Light*) (*_lights)[0])->_color;
            _lightSourceShiness = ((Light*) (*_lights)[0])->_shiness;
        }else{
            _lightSourceColor = glm::vec4(0, 0, 0, 0);
        }

        glm::mat4 lightView = glm::lookAt(-_lightDirection, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

        _lightSpaceMatrix = lightProjection * lightView;

        glUniformMatrix4fv(glGetUniformLocation(_programShadowDepth, "lightSpaceMatrix"), 1, GL_FALSE, &_lightSpaceMatrix[0][0]);

        //

        glUseProgram(_programShadow);

        glUniformMatrix4fv(glGetUniformLocation(_programShadow, "V"), 1, GL_FALSE, &_vTransform[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(_programShadow, "P"), 1, GL_FALSE, &_projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(_programShadow, "lightSpaceMatrix"), 1, GL_FALSE, &_lightSpaceMatrix[0][0]);

        glUniform1i(glGetUniformLocation(_programShadow, "shadowMap"), 1);
        glUniform1i(glGetUniformLocation(_programShadow, "diffuseTexture"), 0);

        glUniform3fv(glGetUniformLocation(_programShadow, "viewPos"), 1, &_cameraPosition[0]);

        glUniform3fv(glGetUniformLocation(_programShadow, "lightDirection"), 1, &_lightDirection[0]);
        glUniform3fv(glGetUniformLocation(_programShadow, "lightSourceColor"), 1, &_lightSourceColor[0]);
        glUniform1f(glGetUniformLocation(_programShadow, "lightSourceShiness"), _lightSourceShiness);

        glUniform3fv(glGetUniformLocation(_programShadow, "ambientLight"), 1, &_ambientLight[0]);

    }else if (_shadow == 2){

        glViewport(0, 0, _SHADOW_WIDTH, _SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBOPointsSource);
        glClear(GL_DEPTH_BUFFER_BIT);

        //

        glUseProgram(_programShadowPointSource);

        if (_lights->size() > 0){
            _lightPosition = ((Light*) (*_lights)[0])->getPosition();
            _lightSourceColor = ((Light*) (*_lights)[0])->_color;
            _lightSourceShiness = ((Light*) (*_lights)[0])->_shiness;
        }else{
            _lightSourceColor = glm::vec4(0, 0, 0, 0);
        }

        glUniform1i(glGetUniformLocation(_programShadowPointSource, "diffuseTexture"), 0);
        glUniform1i(glGetUniformLocation(_programShadowPointSource, "depthMap"), 1);

        glUniform1f(glGetUniformLocation(_programShadowPointSource, "far_plane"), _far_plane);

        glUniform3fv(glGetUniformLocation(_programShadowPointSource, "viewPos"), 1, &_cameraPosition[0]);

        glUniform3fv(glGetUniformLocation(_programShadowPointSource, "lightPos"), 1, &_lightPosition[0]);
        glUniform3fv(glGetUniformLocation(_programShadowPointSource, "lightSourceColor"), 1, &_lightSourceColor[0]);
        glUniform1f(glGetUniformLocation(_programShadowPointSource, "lightSourceShiness"), _lightSourceShiness);

        glUniform3fv(glGetUniformLocation(_programShadowPointSource, "ambientLight"), 1, &_ambientLight[0]);

        glUniformMatrix4fv(glGetUniformLocation(_programShadowPointSource, "V"), 1, GL_FALSE, &_vTransform[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(_programShadowPointSource, "P"), 1, GL_FALSE, &_projection[0][0]);

        //

        glUseProgram(_programShadowDepthPointSource);

        glm::mat4 shadowTransforms[6];
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)_SHADOW_WIDTH / (float)_SHADOW_HEIGHT, _near_plane, _far_plane);
        shadowTransforms[0] = glm::mat4(shadowProj * glm::lookAt(_lightPosition, _lightPosition + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
        shadowTransforms[1] = glm::mat4(shadowProj * glm::lookAt(_lightPosition, _lightPosition + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
        shadowTransforms[2] = glm::mat4(shadowProj * glm::lookAt(_lightPosition, _lightPosition + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)));
        shadowTransforms[3] = glm::mat4(shadowProj * glm::lookAt(_lightPosition, _lightPosition + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)));
        shadowTransforms[4] = glm::mat4(shadowProj * glm::lookAt(_lightPosition, _lightPosition + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
        shadowTransforms[5] = glm::mat4(shadowProj * glm::lookAt(_lightPosition, _lightPosition + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));

        glUniformMatrix4fv(glGetUniformLocation(_programShadowDepthPointSource, "shadowMatrices"), 6, GL_FALSE, &shadowTransforms[0][0][0]);

        glUniform1f(glGetUniformLocation(_programShadowDepthPointSource, "far_plane"), _far_plane);

        glUniform3fv(glGetUniformLocation(_programShadowDepthPointSource, "lightPos"), 1, &_lightPosition[0]);

    }

}

void Renderer::RefreshFrame(){

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, _width, _height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void Renderer::DrawObject(int iteration, bool reflection, bool skyBox, bool explode, bool isAxis, bool drawNormals, GLFWwindow* window, GLuint VertexArrayID, int verticesSize, GLuint textureID, int texture, bool lightSource, glm::vec4 sourceColor){

    if (_shadow == 1){

        switch (iteration){
            case 0:{

                glUseProgram(_programShadowDepth);

                glUniformMatrix4fv(glGetUniformLocation(_programShadowDepth, "M"), 1, GL_FALSE, &_mTransform[0][0]);

                glBindVertexArray(VertexArrayID);
                glDrawArrays(GL_TRIANGLES, 0, verticesSize);

            };
            break;
            case 1:{

                if (skyBox){
                    _program = _programSkyBox;
                }else{
                    _program = _programShadow;
                }

                glUseProgram(_program);

                if (skyBox){
                    glUniformMatrix4fv(glGetUniformLocation(_program, "V"), 1, GL_FALSE, &_vTransform[0][0]);
                    glUniformMatrix4fv(glGetUniformLocation(_program, "P"), 1, GL_FALSE, &_projection[0][0]);
                }

                glUniform1i(glGetUniformLocation(_program, "textureFlag"), texture);

                glUniformMatrix4fv(glGetUniformLocation(_program, "M"), 1, GL_FALSE, &_mTransform[0][0]);

                glUniform3fv(glGetUniformLocation(_program, "ambientModel"), 1, &_modelColorAmbient[0]);
                glUniform3fv(glGetUniformLocation(_program, "diffuseModel"), 1, &_modelColorDiffuse[0]);
                glUniform3fv(glGetUniformLocation(_program, "specularModel"), 1, &_modelColorSpecular[0]);

                if (skyBox){
                    glDepthFunc(GL_LEQUAL);
                }

                glActiveTexture(GL_TEXTURE0);
                if (skyBox){
                    glBindTexture(GL_TEXTURE_2D, _cubemapTexture);
                }else{
                    glBindTexture(GL_TEXTURE_2D, textureID);
                }

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, _depthMap);

                glBindVertexArray(VertexArrayID);
                glDrawArrays(GL_TRIANGLES, 0, verticesSize);

                if (skyBox){
                    glDepthFunc(GL_LESS);
                }

            };
            break;
        }

    }else if (_shadow == 2){

        switch (iteration){
            case 0:{

                glUseProgram(_programShadowDepthPointSource);

                glUniformMatrix4fv(glGetUniformLocation(_programShadowDepthPointSource, "M"), 1, GL_FALSE, &_mTransform[0][0]);

                glBindVertexArray(VertexArrayID);
                glDrawArrays(GL_TRIANGLES, 0, verticesSize);

            };
            break;
            case 1:{

                if (skyBox){
                    _program = _programSkyBox;
                }else{
                    _program = _programShadowPointSource;
                }

                glUseProgram(_program);

                if (skyBox){
                    glUniformMatrix4fv(glGetUniformLocation(_program, "V"), 1, GL_FALSE, &_vTransform[0][0]);
                    glUniformMatrix4fv(glGetUniformLocation(_program, "P"), 1, GL_FALSE, &_projection[0][0]);
                }

                glUniformMatrix4fv(glGetUniformLocation(_program, "M"), 1, GL_FALSE, &_mTransform[0][0]);

                glUniform3fv(glGetUniformLocation(_program, "ambientModel"), 1, &_modelColorAmbient[0]);
                glUniform3fv(glGetUniformLocation(_program, "diffuseModel"), 1, &_modelColorDiffuse[0]);
                glUniform3fv(glGetUniformLocation(_program, "specularModel"), 1, &_modelColorSpecular[0]);

                glUniform1i(glGetUniformLocation(_program, "lightSourceFlag"), (int)lightSource);
                glUniform3fv(glGetUniformLocation(_program, "sourceColor"), 1, &sourceColor[0]);

                glUniform1i(glGetUniformLocation(_program, "textureFlag"), texture);

                if (skyBox){
                    glDepthFunc(GL_LEQUAL);
                }

                glActiveTexture(GL_TEXTURE0);
                if (skyBox){
                    glBindTexture(GL_TEXTURE_2D, _cubemapTexture);
                }else{
                    glBindTexture(GL_TEXTURE_2D, textureID);
                }

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_CUBE_MAP, _depthMapPointsSource);

                glBindVertexArray(VertexArrayID);
                glDrawArrays(GL_TRIANGLES, 0, verticesSize);

                if (skyBox){
                    glDepthFunc(GL_LESS);
                }

            };
            break;
        }

    }else{

        glEnable(GL_DEPTH_TEST);

        // Shaders

        if (_explode && explode){
            _program = _programExplode;
        }else if (skyBox){
            _program = _programSkyBox;
        }else if (reflection && texture == 4){
            _program = _programReflection;
        }else{
            _program = _programPhong;
        }
        glUseProgram(_program);

        // Transform matrices

        glm::mat4 mvp = _projection * _vTransform * _mTransform;
        glUniformMatrix4fv(glGetUniformLocation(_program, "MVP"), 1, GL_FALSE, &mvp[0][0]);

        glUniformMatrix4fv(glGetUniformLocation(_program, "M"), 1, GL_FALSE, &_mTransform[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(_program, "V"), 1, GL_FALSE, &_vTransform[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(_program, "P"), 1, GL_FALSE, &_projection[0][0]);

        // Model color

        glUniform3fv(glGetUniformLocation(_program, "ambientModel"), 1, &_modelColorAmbient[0]);
        glUniform3fv(glGetUniformLocation(_program, "diffuseModel"), 1, &_modelColorDiffuse[0]);
        glUniform3fv(glGetUniformLocation(_program, "specularModel"), 1, &_modelColorSpecular[0]);

        // Light

        glUniform3fv(glGetUniformLocation(_program, "ambientLight"), 1, &_ambientLight[0]);

        int lightSources = _lights->size();

        glUniform1i(glGetUniformLocation(_program, "numberLightSources"), lightSources);

        glm::vec3 lightsColor[lightSources];
        glm::vec3 lightsPosition[lightSources];
        int lightsType[lightSources];
        int lightsShiness[lightSources];

        for (int i = 0; i < _lights->size(); i++){

            Light* light = (Light*) (*_lights)[i];

            lightsType[i] = light->_sourceType;
            lightsColor[i] = light->_color;
            lightsPosition[i] = light->_sourceType == 0 ? light->getPosition() : light->getDirection();
            lightsShiness[i] = light->_shiness;

        }

        glUniform3fv(glGetUniformLocation(_program, "LightPosition_worldspace"), lightSources, &lightsPosition[0][0]);
        glUniform3fv(glGetUniformLocation(_program, "lightSourceColor"), lightSources, &lightsColor[0][0]);
        glUniform1iv(glGetUniformLocation(_program, "lightSourceType"), lightSources, &lightsType[0]);
        glUniform1iv(glGetUniformLocation(_program, "lightSourceShiness"), lightSources, &lightsShiness[0]);

        // Other

        glUniform3fv(glGetUniformLocation(_program, "eye_position"), 1, &_cameraPosition[0]);

        glUniform1i(glGetUniformLocation(_program, "textureFlag"), texture);

        glUniform1i(glGetUniformLocation(_program, "fogFlag"), (int)_fog);
        glUniform1f(glGetUniformLocation(_program, "FogDensity"), _fogFactor);

        glUniform1i(glGetUniformLocation(_program, "depthFlag"), (int)_depth);

        glUniform1i(glGetUniformLocation(_program, "lightSourceFlag"), (int)lightSource);
        glUniform3fv(glGetUniformLocation(_program, "sourceColor"), 1, &sourceColor[0]);

        glUniform1f(glGetUniformLocation(_program, "time"), glfwGetTime());

        glUniform1i(glGetUniformLocation(_program, "myTextureSampler0"), 0);

        // Draw Object

        if (skyBox){
            glDepthFunc(GL_LEQUAL);
        }

        if (_bloom){
            glBindFramebuffer(GL_FRAMEBUFFER, _framebufferBloom);
        }else if (_aliasing){
            glBindFramebuffer(GL_FRAMEBUFFER, _framebufferAntialiasing);
        }else{
            glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
        }

        glActiveTexture(GL_TEXTURE0);
        if (skyBox){
            glBindTexture(GL_TEXTURE_2D, _cubemapTexture);
        }else{
            glBindTexture(GL_TEXTURE_2D, textureID);
        }

        glBindVertexArray(VertexArrayID);
        glDrawArrays(GL_TRIANGLES, 0, verticesSize);

        if (skyBox){
            glDepthFunc(GL_LESS);
        }

        // Draw normals

        if (drawNormals){

            _program = _programNormals;
            glUseProgram(_program);

            glUniformMatrix4fv(glGetUniformLocation(_program, "M"), 1, GL_FALSE, &_mTransform[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(_program, "V"), 1, GL_FALSE, &_vTransform[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(_program, "P"), 1, GL_FALSE, &_projection[0][0]);

            glUniform1i(glGetUniformLocation(_program, "axis"), int(isAxis));

            glBindVertexArray(VertexArrayID);
            glDrawArrays(GL_TRIANGLES, 0, verticesSize);

        }

    }

}

void Renderer::DrawFrame(){

    if (_shadow == 1){

        glUseProgram(_programShadowScreen);
        glUniform1i(glGetUniformLocation(_programShadowScreen, "depthMap"), 0);
        glUniform1f(glGetUniformLocation(_programShadowScreen, "near_plane"), _near_plane);
        glUniform1f(glGetUniformLocation(_programShadowScreen, "far_plane"), _far_plane);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _depthMap);

    }else if (_shadow == 2){

















    }else if (_bloom){

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        bool horizontal = true, first_iteration = true;
        unsigned int amount = 4;

        glUniform1i(glGetUniformLocation(_programBlur, "image"), 0);
        glUseProgram(_programBlur);

        for (unsigned int i = 0; i < amount; i++)
        {

            glBindFramebuffer(GL_FRAMEBUFFER, _pingpongFBO[horizontal]);
            glUniform1i(glGetUniformLocation(_programBlur, "horizontal"), (int)horizontal);
            glUniform1i(glGetUniformLocation(_programBlur, "iteration"), (i/2)%2);

            glBindTexture(GL_TEXTURE_2D, first_iteration ? _colorBuffersBloom[1] : _pingpongColorbuffers[!horizontal]);

            glBindVertexArray(_screenID);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            horizontal = !horizontal;

            if (first_iteration)
                first_iteration = false;

        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(_programBloomfinal);

        glUniform1f(glGetUniformLocation(_programBloomfinal, "offsetX"), _offsetX);
        glUniform1f(glGetUniformLocation(_programBloomfinal, "offsetY"), _offsetY);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, _pingpongColorbuffers[!horizontal]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _colorBuffersBloom[0]);

        glUniform1i(glGetUniformLocation(_programBloomfinal, "bloom"), true);
        glUniform1f(glGetUniformLocation(_programBloomfinal, "exposure"), _exposure);

        glUniform1i(glGetUniformLocation(_programBloomfinal, "scene"), 0);
        glUniform1i(glGetUniformLocation(_programBloomfinal, "bloomBlur"), 1);

        glBindVertexArray(_screenID);
        glDrawArrays(GL_TRIANGLES, 0, 6);

    }else if (_aliasing){

        glBindFramebuffer(GL_READ_FRAMEBUFFER, _framebufferAntialiasing);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _framebuffer);
        glBlitFramebuffer(0, 0, _width, _height, 0, 0, _width, _height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(_programScreen);
        glUniform1i(glGetUniformLocation(_programScreen, "postProcessing"), _postProcessing);

        glBindVertexArray(_screenID);
        glBindTexture(GL_TEXTURE_2D, _colorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glEnable(GL_DEPTH_TEST);

    }else{

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(_programScreen);
        glUniform1i(glGetUniformLocation(_programScreen, "postProcessing"), _postProcessing);

        glBindVertexArray(_screenID);
        glBindTexture(GL_TEXTURE_2D, _colorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glEnable(GL_DEPTH_TEST);

    }

}
