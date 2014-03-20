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
#include "SkyBoxMaterial.h"
#include "ScreenAlignedQuadMaterial.h"
#include "Camera.h"
#include "Mesh.h"

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
	SkyBoxMaterial _skyBoxMaterial;
	ScreenAlignedQuadMaterial _screenAlignedMaterial;

	Camera _mainCamera;
	Camera _secondCamera;

	//параметры освещения
	float _lightTheta;
	float _lightPhi;
	float _lightR;
	glm::vec4 _lightPos; //in world space
	glm::vec3 _ambientColor;
	glm::vec3 _diffuseColor;
	glm::vec3 _specularColor;

	//идентификаторы текстурных объектов
	GLuint _worldTexId;
	GLuint _brickTexId;
	GLuint _grassTexId;	
	GLuint _specularTexId;
	GLuint _chessTexId;
	GLuint _myTexId;
	GLuint _cubeTexId;
	GLuint _colorTexId;

	//параметры чтения из текстуры
	GLuint _sampler;
	GLuint _repeatSampler;
	GLuint _cubeSampler;

	//полигональные 3д-модели
	Mesh _sphere;
	Mesh _cube;
	Mesh _backgroundCube;
	Mesh _plane;
	Mesh _chess;
	Mesh _bunny;
	Mesh _screenQuad;

	float _oldTime;

	int _width;
	int _height;

	TwBar* _bar;
					
	void makeSceneImplementation();
	void drawScene(Camera& camera);
	void drawBackground(Camera& camera);
};