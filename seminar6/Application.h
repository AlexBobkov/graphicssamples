#pragma once

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <AntTweakBar.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct CameraInfo
{
	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;
};

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
	Инициализирует графический интерфейс пользователя
	*/
	virtual void initGUI();

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
	
	CameraInfo _camera;

	//Положение виртуальный камеры задается в сферических координат
	double _phiAng;
	double _thetaAng;
	double _r;

	double _oldTime; //Время на предыдущем кадре

	//Вспомогальные переменные для управления виртуальной камерой
	bool _rotateLeft;
	bool _rotateRight;	
	bool _rotateUp;
	bool _rotateDown;
	bool _radiusInc;
	bool _radiusDec;

	TwBar* _bar; //GUI
};
