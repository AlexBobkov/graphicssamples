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

	void rotateLeft(bool b) { _rotateLeft = b; }
	void rotateRight(bool b) { _rotateRight = b; }

	void rotateUp(bool b) { _rotateUp = b; }
	void rotateDown(bool b) { _rotateDown = b; }

	void fovInc(bool b) { _fovInc = b; }
	void fovDec(bool b) { _fovDec = b; }

protected:	
	GLFWwindow* _window;
	GLuint _vao;
	GLuint _shaderProgram;

	GLuint _modelMatrixUniform;
	GLuint _viewMatrixUniform;
	GLuint _projMatrixUniform;
	GLuint _timeUniform;

	glm::mat4 _modelMatrix;
	glm::mat4 _viewMatrix;
	glm::mat4 _projMatrix;

	double _oldTime;

	bool _rotateLeft;
	bool _rotateRight;
	double _phiAng;

	bool _rotateUp;
	bool _rotateDown;
	double _thetaAng;

	double _z;

	bool _fovInc;
	bool _fovDec;
	float _fov;

	//Читает текст шейдера из файла и создает объект
	GLuint createShader(GLenum shaderType, std::string filename);

	//Конкретные реализация для разных примеров на семинаре (треугольник, куб)
	void makeSceneImplementation();
	void makeSceneImplementationLine();
	void makeSceneImplementationQuad();

	void drawImplementation();
	void drawImplementationLine();
	void drawImplementationQuad();
};