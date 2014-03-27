#pragma once

#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AntTweakBar.h>

#include "CommonMaterial.h"
#include "ScreenAlignedQuadMaterial.h"
#include "ShadowMaterial.h"
#include "RenderToShadowMaterial.h"
#include "ColorMaterial.h"
#include "Camera.h"
#include "Mesh.h"
#include "Light.h"

class Application
{
public:
	Application();
	~Application();

	//Инициализация графического контекста
	void initContext();

	//Настройка некоторых параметров OpenGL
	void initGL();

	void initOthers();

	//Создание трехмерной сцены
	void makeScene();
		
	//Цикл рендеринга
	void run();

	//Отрисовать один кадр
	void draw();

	//Обновление
	void update();	

	Camera& getMainCamera() { return _mainCamera; }

	void setWindowSize(int width, int height);

protected:
	GLFWwindow* _window;
	
	CommonMaterial _commonMaterial;	
	ScreenAlignedQuadMaterial _screenAlignedMaterial;
	ShadowMaterial _shadowMaterial;
	RenderToShadowMaterial _renderToShadowMaterial;
	ColorMaterial _colorMaterial;

	Camera _mainCamera;
	Camera _lightCamera;

	//параметры освещения
	float _lightTheta;
	float _lightPhi;
	float _lightR;
	Light _light;

	//идентификаторы текстурных объектов
	GLuint _worldTexId;
	GLuint _brickTexId;
	GLuint _grassTexId;	
	GLuint _specularTexId;
	GLuint _chessTexId;
	GLuint _myTexId;
	GLuint _cubeTexId;
	GLuint _colorTexId;
	GLuint _renderTexId;
	GLuint _depthTexId;

	//параметры чтения из текстуры
	GLuint _sampler;
	GLuint _repeatSampler;
	GLuint _cubeSampler;

	//полигональные 3д-модели
	Mesh _sphere;
	Mesh _cube;
	Mesh _backgroundCube;
	Mesh _plane;
	Mesh _ground;
	Mesh _bunny;
	Mesh _screenQuad;
	Mesh _sphereMarker;

	GLuint _framebufferId;
	int _fbWidth;
	int _fbHeight;

	float _oldTime;

	int _width;
	int _height;

	TwBar* _bar;
					
	void makeSceneImplementation();

	void initShadowFramebuffer();

	void drawToShadowMap(Camera& lightCamera);
	void drawSceneWithShadow(Camera& mainCamera, Camera& lightCamera);

	//void drawMultiObjectScene(Camera& camera);
};