#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

class Application
{
public:
	Application();
	~Application();

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
	void makeScene();

	/**
	Запускает цикл рендеринга
	*/
	void run();

	/**
	Отрисовывает один кадр
	*/
	void draw();
		
protected:
	//Читает текст шейдера из файла и создает объект
	GLuint createShader(GLenum shaderType, std::string filename);
	
	GLFWwindow* _window; //Графичекое окно

	GLuint _vao;
	GLuint _shaderProgram;
	GLuint _projMatrixUniform;
	float _projMatrix[16];	

	//Конкретные реализация для разных примеров на семинаре (треугольник, куб)
	void makeSceneImplementation();
	void makeSceneImplementation2();
	void makeSceneImplementation3();
	void makeSceneImplementation4();

	void drawImplementation();
	void drawImplementation3();
	void drawImplementation4();
};