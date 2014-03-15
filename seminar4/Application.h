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

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

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

protected:	
	void makeSphere();
	void makeCube();
	void makeShaders();

	GLFWwindow* _window;

	GLuint _shaderProgram;

	//идентификаторы uniform-переменных
	GLuint _timeUniform;
	GLuint _modelMatrixUniform;
	GLuint _viewMatrixUniform;
	GLuint _projMatrixUniform;	
	GLuint _normalToCameraMatrixUniform;
	GLuint _lightDirUniform;
	GLuint _lightPosUniform;
	GLuint _ambientColorUniform;
	GLuint _diffuseColorUniform;
	GLuint _specularColorUniform;
	GLuint _shininessUniform;
	GLuint _materialUniform;
	GLuint _attenuationUniform;

	//переменные, которые содержат значения, которые будут записаны в uniform-переменные шейдеров
	glm::mat4 _viewMatrix;
	glm::mat4 _projMatrix;
	glm::mat3 _normalToCameraMatrix;
	glm::vec4 _lightDir; //in world space
	glm::vec4 _lightPos; //in world space
	glm::vec3 _ambientColor;
	glm::vec3 _diffuseColor;
	glm::vec3 _specularColor;
	float _attenuation;
		
	//sphere
	GLuint _sphereVao;	
	glm::mat4 _sphereModelMatrix;
	int _sphereNumTris;
	float _sphereShininess;
	glm::vec3 _sphereMaterial;

	//cube
	GLuint _cubeVao;
	glm::mat4 _cubeModelMatrix;
	int _cubeNumTris;
	float _cubeShininess;
	glm::vec3 _cubeMaterial;

	double _oldTime;

	//Состояние виртуальной камеры
	bool _rotateLeft;
	bool _rotateRight;
	double _phiAng;

	bool _rotateUp;
	bool _rotateDown;
	double _thetaAng;

	//Читает текст шейдера из файла и создает объект
	GLuint createShader(GLenum shaderType, std::string filename);
		
	void makeSceneImplementation();
	void drawImplementation();
};