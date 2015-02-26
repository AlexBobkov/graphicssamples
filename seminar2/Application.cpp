#include "Application.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>

//======================================

//Функция обратного вызова для обработки нажатий на клавиатуре
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Application* app = (Application*)glfwGetWindowUserPointer(window);

	app->handleKey(key, scancode, action, mods);
}

//======================================

Application::Application():
_oldTime(0.0),
	_rotateLeft(false),
	_rotateRight(false),
	_phiAng(0.0),
	_rotateUp(false),
	_rotateDown(false),
	_thetaAng(0.0),
	_radiusInc(false),
	_radiusDec(false),
	_r(5.0)
{
}

Application::~Application()
{
	glfwTerminate();
}

void Application::start()
{
	initContext();
	initGL();
	makeScene();
	run();
}

void Application::initContext()
{
	if (!glfwInit())
	{
		std::cerr << "ERROR: could not start GLFW3\n";
		exit(1);
	}

#ifdef USE_CORE_PROFILE
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

	_window = glfwCreateWindow(640, 480, "MIPT OpenGL demos", NULL, NULL);
	if (!_window)
	{
		std::cerr << "ERROR: could not open window with GLFW3\n";
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(_window);

	glfwSetKeyCallback(_window, keyCallback); //Регистрирует функцию обратного вызова для обработки событий клавиатуры
	glfwSetWindowUserPointer(_window, this); //Регистрируем указатель на данный объект, чтобы потом использовать его в функциях обратного вызова}
}

void Application::initGL()
{
	glewExperimental = GL_TRUE;
	glewInit();

	const GLubyte* renderer = glGetString(GL_RENDERER); //Получаем имя рендерера
	const GLubyte* version = glGetString(GL_VERSION); //Получаем номер версии
	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "OpenGL version supported: " << version << std::endl;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void Application::makeScene()
{
	_viewMatrix = glm::lookAt(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	_projMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
}

void Application::run()
{
	while (!glfwWindowShouldClose(_window)) //Пока окно не закрыто
	{
		glfwPollEvents(); //Проверяем события ввода

		update(); //Обновляем сцену и положение виртуальной камеры

		draw(); //Рисуем один кадр

		glfwSwapBuffers(_window); //Переключаем передний и задний буферы
	}
}

void Application::handleKey(int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(_window, GL_TRUE);
		}
		else if (key == GLFW_KEY_A)
		{
			_rotateLeft = true;
		}
		else if (key == GLFW_KEY_D)
		{
			_rotateRight = true;
		}
		else if (key == GLFW_KEY_W)
		{
			_rotateUp = true;
		}
		else if (key == GLFW_KEY_S)
		{
			_rotateDown = true;
		}
		else if (key == GLFW_KEY_R)
		{
			_radiusInc = true;
		}
		else if (key == GLFW_KEY_F)
		{
			_radiusDec = true;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_A)
		{
			_rotateLeft = false;
		}
		else if (key == GLFW_KEY_D)
		{
			_rotateRight = false;
		}
		else if (key == GLFW_KEY_W)
		{
			_rotateUp = false;
		}
		else if (key == GLFW_KEY_S)
		{
			_rotateDown = false;
		}
		else if (key == GLFW_KEY_R)
		{
			_radiusInc = false;
		}
		else if (key == GLFW_KEY_F)
		{
			_radiusDec = false;
		}
	}
}

void Application::update()
{
	double dt = glfwGetTime() - _oldTime;
	_oldTime = glfwGetTime();

	double speed = 1.0;

	if (_rotateLeft)
	{
		_phiAng -= speed * dt;
	}
	if (_rotateRight)
	{
		_phiAng += speed * dt;
	}
	if (_rotateUp)
	{
		_thetaAng += speed * dt;
	}
	if (_rotateDown)
	{
		_thetaAng -= speed * dt;
	}
	if (_radiusInc)
	{
		_r += _r * dt;
	}
	if (_radiusDec)
	{
		_r -= _r * dt;
	}

	//Вычисляем положение виртуальной камеры в мировой системе координат по формуле сферических координат
	glm::vec3 pos = glm::vec3(glm::cos(_phiAng) * glm::cos(_thetaAng), glm::sin(_phiAng) * glm::cos(_thetaAng), glm::sin(_thetaAng)) * (float)_r;

	//Обновляем матрицу вида
	_viewMatrix = glm::lookAt(pos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));


	//-----------------------------------------

	int width, height;
	glfwGetFramebufferSize(_window, &width, &height);	

	//Обновляем матрицу проекции на случай, если размеры окна изменились
	_projMatrix = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.f);
}
