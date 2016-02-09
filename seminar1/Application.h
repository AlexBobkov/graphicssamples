#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Application
{
public:
	Application();
	~Application();

	/**
	Запускает приложение
	*/
	void start();	
		
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
	virtual void makeScene();

	/**
	Запускает цикл рендеринга
	*/
	void run();

	/**
	Отрисовывает один кадр
	*/
	virtual void draw() = 0;

	/**
	Вспомогательная функция для загрузки текста шейдера из файла и создания шейдерного объекта
	*/
	GLuint createShader(GLenum shaderType, const std::string& filename);

	/**
	Вспомогательная функция для создания шейдерной программы из 2х шейдеров: вершинного и фрагментного
	*/
	GLuint createProgram(const std::string& vertFilename, const std::string& fragFilename);
	
	GLFWwindow* _window; //Графичекое окно

	glm::mat4 _projMatrix;
};