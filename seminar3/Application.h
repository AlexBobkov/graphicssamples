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

	/**
	Обрабатывает нажатия кнопок на клавитуре.
	См. сигнатуру GLFWkeyfun библиотеки GLFW
	*/
	virtual void handleKey(int key, int scancode, int action, int mods);

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
	Выполняет обновление сцены и виртуальной камеры
	*/
	virtual void update();

	/**
	Отрисовывает один кадр
	*/
	virtual void draw() = 0;	

	//---------------------------------------------

	GLFWwindow* _window; //Графичекое окно
	
	glm::mat4 _viewMatrix;
	glm::mat4 _projMatrix;	

	double _phiAng;
	double _thetaAng;
	double _r;

	double _oldTime;

	bool _rotateLeft;
	bool _rotateRight;	

	bool _rotateUp;
	bool _rotateDown;

	bool _radiusInc;
	bool _radiusDec;
};
