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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "CommonMaterial.h"
#include "SkyBoxMaterial.h"
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

	//Создание трехмерной сцены
	void makeScene();
		
	//Цикл рендеринга
	void run();

	//Отрисовать один кадр
	void draw();

	//Обновление
	void update();	

protected:
	GLFWwindow* _window;
	
	CommonMaterial _commonMaterial;	
	SkyBoxMaterial _skyBoxMaterial;

	Camera _mainCamera;
	Camera _secondCamera;

	//параметры освещения
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

	//параметры чтения из текстуры
	GLuint _sampler;
	GLuint _repeatSampler;
	GLuint _cubeSampler;

	//полигональные 3д-модели
	Mesh _sphere;
	Mesh _cube;
	Mesh _plane;
	Mesh _chess;

	float _oldTime;
					
	void makeSceneImplementation();
	void drawScene(Camera& camera);
	void drawBackground(Camera& camera);
};