#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "Application.h"

int demoNum = 1;

//Функция обратного вызова для обработки нажатий на клавиатуре
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Application* app = (Application*)glfwGetWindowUserPointer(window);

	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if (key == GLFW_KEY_A)
		{
			app->rotateLeft(true);
		}
		else if (key == GLFW_KEY_D)
		{
			app->rotateRight(true);
		}
		else if (key == GLFW_KEY_W)
		{
			app->rotateUp(true);
		}
		else if (key == GLFW_KEY_S)
		{
			app->rotateDown(true);
		}
	}
	else if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_A)
		{
			app->rotateLeft(false);
		}
		else if (key == GLFW_KEY_D)
		{
			app->rotateRight(false);
		}
		else if (key == GLFW_KEY_W)
		{
			app->rotateUp(false);
		}
		else if (key == GLFW_KEY_S)
		{
			app->rotateDown(false);
		}
	}
}

//======================================

Application::Application():
_oldTime(0.0f),
	_rotateLeft(false),
	_rotateRight(false),
	_phiAng(0.0f),
	_rotateUp(false),
	_rotateDown(false),
	_thetaAng(0.0f)	
{
}

Application::~Application()
{
	glfwTerminate();
}

void Application::initContext()
{
	if (!glfwInit())
	{
		std::cerr << "ERROR: could not start GLFW3\n";		
		exit(1);
	} 

	_window = glfwCreateWindow(640, 480, "Hello Triangle", NULL, NULL);
	if (!_window)
	{
		std::cerr << "ERROR: could not open window with GLFW3\n";		
		glfwTerminate();
		exit(1);
	}
	glfwMakeContextCurrent(_window);

	glfwSetWindowUserPointer(_window, this); //регистрируем указатель на данный объект, чтобы потом использовать его в функциях обратного вызова
}

void Application::initGL()
{
	glewExperimental = GL_TRUE;
	glewInit ();

	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "OpenGL version supported: " << version << std::endl;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void Application::makeScene()
{
	makeSceneImplementation();
}

void Application::run()
{
	glfwSetKeyCallback(_window, keyCallback);

	while (!glfwWindowShouldClose(_window))
	{
		glfwPollEvents();

		update();

		draw();
	}
}

void Application::draw()
{
	int width, height;
	glfwGetFramebufferSize(_window, &width, &height);
	glViewport(0, 0, width, height);		
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_projMatrix = glm::perspective(45.0f, (float)width / height, 0.1f, 100.f);

	drawImplementation();

	glfwSwapBuffers(_window);	
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

	glm::vec3 pos = glm::vec3(glm::cos(_phiAng) * glm::cos(_thetaAng), glm::sin(_phiAng) * glm::cos(_thetaAng), glm::sin(_thetaAng)) * 5.0f;

	_viewMatrix = glm::lookAt(pos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Application::makeSceneImplementation()
{
	makeSphere();
	makeCube();
	makePlane();
	makeShaders();
	initData();
}

void Application::makeShaders()
{
	/*std::string vertFilename = "shaders5/shader.vert";
	std::string fragFilename = "shaders5/shader.frag";*/

	std::string vertFilename = "shaders5/texture_without_lighting.vert";
	std::string fragFilename = "shaders5/texture_without_lighting.frag";

	_shaderProgram = makeShaderProgram(vertFilename, fragFilename);

	//=========================================================
	//Инициализация uniform-переменных для преобразования координат

	_timeUniform = glGetUniformLocation(_shaderProgram, "time");
	_modelMatrixUniform = glGetUniformLocation(_shaderProgram, "modelMatrix");
	_viewMatrixUniform = glGetUniformLocation(_shaderProgram, "viewMatrix");
	_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");
	_normalToCameraMatrixUniform = glGetUniformLocation(_shaderProgram, "normalToCameraMatrix");	

	//=========================================================
	//Инициализация uniform-переменных для освещения

	_lightDirUniform = glGetUniformLocation(_shaderProgram, "lightDir");
	_lightPosUniform = glGetUniformLocation(_shaderProgram, "lightPos");
	_ambientColorUniform = glGetUniformLocation(_shaderProgram, "ambientColor");
	_diffuseColorUniform = glGetUniformLocation(_shaderProgram, "diffuseColor");
	_specularColorUniform = glGetUniformLocation(_shaderProgram, "specularColor");
	_shininessUniform = glGetUniformLocation(_shaderProgram, "shininessFactor");
	_materialUniform = glGetUniformLocation(_shaderProgram, "material");
	_attenuationUniform = glGetUniformLocation(_shaderProgram, "attenuation");

	//=========================================================
	//Инициализация uniform-переменных для текстурирования

	_diffuseTexUniform = glGetUniformLocation(_shaderProgram, "diffuseTex");	
}

void Application::initData()
{
	//Инициализация матриц
	_viewMatrix = glm::lookAt(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	_projMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.f);

	//Инициализация значений переменных освщения
	_lightDir = glm::vec4(0.0f, 1.0f, 0.8f, 0.0f);
	_lightPos = glm::vec4(0.0f, 1.0f, 0.8f, 1.0f);
	_ambientColor = glm::vec3(0.2, 0.2, 0.2);
	_diffuseColor = glm::vec3(0.8, 0.8, 0.8);
	_specularColor = glm::vec3(0.25, 0.25, 0.25);
	_attenuation = 1.0f;

	//Инициализация материалов
	_shininess1 = 100.0f;
	_material1 = glm::vec3(1.0, 0.0, 0.0);

	_shininess2 = 100.0f;	
	_material2 = glm::vec3(0.0, 1.0, 0.0);	

	//Инициализация текстур
	_worldTexId = makeTexture("images/earth_global.jpg");
	_brickTexId = makeTexture("images/brick.jpg");
	_grassTexId = makeTexture("images/grass.jpg");
}

void Application::drawImplementation()
{
	glUseProgram(_shaderProgram);

	//=============================================================================
	//Копирование на видеокарту значений uniform-пемеренных, общих для всех объектов

	glUniform1f(_timeUniform, (float)glfwGetTime()); //передаем время в шейдер	
	glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
	glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));

	glUniform4fv(_lightDirUniform, 1, glm::value_ptr(_lightDir));
	glUniform4fv(_lightPosUniform, 1, glm::value_ptr(_lightPos));
	glUniform3fv(_ambientColorUniform, 1, glm::value_ptr(_ambientColor));
	glUniform3fv(_diffuseColorUniform, 1, glm::value_ptr(_diffuseColor));
	glUniform3fv(_specularColorUniform, 1, glm::value_ptr(_specularColor));
	glUniform1f(_attenuationUniform, _attenuation);

	glUniform1i(_diffuseTexUniform, 0); //текстурный юнит 0

		
	//=============================================================================
	//Активизация текстур
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _worldTexId);


	//====== Сфера ======
	//Копирование на видеокарту значений uniform-пемеренных для сферы
	_normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * _sphereModelMatrix)));
	glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(_normalToCameraMatrix));

	glUniform3fv(_materialUniform, 1, glm::value_ptr(_material1));
	glUniform1f(_shininessUniform, _shininess1);

	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_sphereModelMatrix));	

	glBindVertexArray(_sphereVao); //Подключаем VertexArray для сферы
	glDrawArrays(GL_TRIANGLES, 0, _sphereNumTris * 3); //Рисуем сферу

	//====== Плоскость ======
	//Копирование на видеокарту значений uniform-пемеренных для плоскости
	_normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * _planeModelMatrix)));
	glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(_normalToCameraMatrix));

	glUniform3fv(_materialUniform, 1, glm::value_ptr(_material2));
	glUniform1f(_shininessUniform, _shininess2);

	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_planeModelMatrix));

	glBindVertexArray(_planeVao); //Подключаем VertexArray для плоскости
	glDrawArrays(GL_TRIANGLES, 0, 6); //Рисуем плоскость

#if 0
	//====== Куб ======
	//Копирование на видеокарту значений uniform-пемеренных для куба
	_normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * _cubeModelMatrix)));
	glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(_normalToCameraMatrix));

	glUniform3fv(_materialUniform, 1, glm::value_ptr(_material2));
	glUniform1f(_shininessUniform, _shininess2);

	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_cubeModelMatrix));

	glBindVertexArray(_cubeVao); //Подключаем VertexArray для куба
	glDrawArrays(GL_TRIANGLES, 0, _cubeNumTris * 3); //Рисуем куб
#endif
}