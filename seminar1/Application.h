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

protected:	
	GLFWwindow* _window;
	GLuint _vao;
	GLuint _shaderProgram;
	GLuint _projMatrixUniform;
	float _projMatrix[16];

	//Читает текст шейдера из файла и создает объект
	GLuint createShader(GLenum shaderType, std::string filename);

	//Конкретные реализация для разных примеров на семинаре (треугольник, куб)
	void makeSceneImplementation();
	void makeSceneImplementation2();
	void makeSceneImplementation3();
	void makeSceneImplementation4();

	void drawImplementation();
	void drawImplementation3();
	void drawImplementation4();
};