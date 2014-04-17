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

#include <OVR.h>

#include "OculusDistortionShader.h"
#include "ColorMaterial.h"
#include "CommonMaterial.h"
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

	void initOVR();
	void destroyOVR();

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
	
	//Классы для загрузи шейдеров для разных материалов и эффектов
	ColorMaterial _colorMaterial;	
	CommonMaterial _commonMaterial;
	OculusDistortionShader _oculusDistortionShader;

	Camera _mainCamera;
	Camera _lightCamera;

	//параметры освещения
	float _lightTheta;
	float _lightPhi;
	float _lightR;
	float _ambientIntensity;
	float _diffuseIntensity;
	float _specularIntensity;
	Light _light;

	//идентификаторы текстурных объектов
	GLuint _worldTexId;
	GLuint _brickTexId;
	GLuint _grassTexId;
	GLuint _colorTexId;
	GLuint _renderTexId;

	//================================================================

	//параметры чтения из текстуры
	GLuint _sampler; //линейная фильтрация
	GLuint _pixelPreciseSampler; //фильтрация NEAREST
	GLuint _repeatSampler;

	//полигональные 3д-модели
	Mesh _sphere;
	Mesh _cube;
	Mesh _backgroundCube;
	Mesh _plane;
	Mesh _ground;
	Mesh _bunny;
	Mesh _teapot;
	Mesh _screenQuad;
	Mesh _sphereMarker;
	Mesh _sphereArray;

	std::vector<glm::vec3> _positions;

	float _oldTime;
	float _fps;
	float _deltaTime;

	int _width;
	int _height;

	TwBar* _bar;

	OVR::Ptr<OVR::DeviceManager> _pManager;
	OVR::Ptr<OVR::HMDDevice> _pHMD;
	OVR::Ptr<OVR::SensorDevice>	_pSensor;
	OVR::SensorFusion* _pFusionResult;
	OVR::HMDInfo _info;
	bool _infoLoaded;
	float _projectionCenterOffset;

	GLuint _framebufferId;
	int _fbWidth;
	int _fbHeight;
					
	void makeSceneImplementation();

	void initFramebuffer();

	void drawScene(glm::mat4& viewMat, glm::mat4& projMat);
	void drawPostprocess();
};