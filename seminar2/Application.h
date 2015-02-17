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
#include <cstdlib>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

class Application
{
public:
	Application();
	~Application();

	/**
	Запускает приложение
	*/
	void start();	

	

	void rotateLeft(bool b) { _rotateLeft = b; }
	void rotateRight(bool b) { _rotateRight = b; }

	void rotateUp(bool b) { _rotateUp = b; }
	void rotateDown(bool b) { _rotateDown = b; }

	void fovInc(bool b) { _fovInc = b; }
	void fovDec(bool b) { _fovDec = b; }

protected:
	/**
	Инициализирует графический контекст
	*/
	void initContext();

	/**
	Настраивает некоторые параметры OpenGL
	*/
	void initGL();

	/**
	Создает трехмерную сцену
	*/
	virtual void makeScene() = 0;

	/**
	Запускает цикл рендеринга
	*/
	void run();

	/**
	Отрисовывает один кадр
	*/
	virtual void draw() = 0;

	/**
	Выполняет обновление сцены и других сущностей
	*/
	void update();

	/**
	Вспомогательная функция для загрузки текста шейдера из файла и создания шейдерного объекта
	*/
	GLuint createShader(GLenum shaderType, const std::string& filename);

	/**
	Вспомогательная функция для создания шейдерной программы из 2х шейдеров: вершинного и фрагментного
	*/
	GLuint createProgram(const std::string& vertFilename, const std::string& fragFilename);

	GLFWwindow* _window; //Графичекое окно

	GLuint _viewMatrixUniform;
	GLuint _projMatrixUniform;
	
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
};
