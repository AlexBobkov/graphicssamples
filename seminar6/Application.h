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

#include "CommonShader.h"
#include "SkyBoxShader.h"

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

	//Функции для управления положением камеры
	void rotateLeft(bool b) { _rotateLeft = b; }
	void rotateRight(bool b) { _rotateRight = b; }

	void rotateUp(bool b) { _rotateUp = b; }
	void rotateDown(bool b) { _rotateDown = b; }

	void zoomUp(bool b) { _zoomUp = b; }
	void zoomDown(bool b) { _zoomDown = b; }

	void homePos();

protected:	
	void initData();

	GLuint makeSphere(float radius, int& numTris) const;	
	GLuint makeCube(float size, int& numTris) const;
	GLuint makePlane(int& numTris) const;
	GLuint makeChessPlane(int& numTris) const;
	
	GLuint loadTexture(std::string filename) const;
	GLuint loadTextureWithMipmaps(std::string filename) const;
	GLuint makeCustomTexture() const;

	GLFWwindow* _window;
	
	CommonShader _commonShader;	
	SkyBoxShader _skyBoxShader;

	//переменные, которые содержат значения, которые будут записаны в uniform-переменные шейдеров
	glm::mat4 _viewMatrix;
	glm::mat4 _projMatrix;
	glm::vec3 _cameraPos;
	
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

	GLuint _sampler;
	GLuint _repeatSampler;
			
	//sphere
	GLuint _sphereVao;	
	glm::mat4 _sphereModelMatrix;
	int _sphereNumTris;

	//cube
	GLuint _cubeVao;
	glm::mat4 _cubeModelMatrix;
	int _cubeNumTris;

	//plane
	GLuint _planeVao;
	glm::mat4 _planeModelMatrix;	
	int _planeNumTris;

	//chess
	GLuint _chessVao;
	int _chessNumTris;

	float _oldTime;

	//Состояние виртуальной камеры
	bool _rotateLeft;
	bool _rotateRight;
	float _phiAng;

	bool _rotateUp;
	bool _rotateDown;
	float _thetaAng;

	bool _zoomUp;
	bool _zoomDown;
	float _distance;
				
	void makeSceneImplementation();
	void drawImplementation();
};