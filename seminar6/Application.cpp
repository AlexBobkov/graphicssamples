#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "Application.h"

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
		else if (key == GLFW_KEY_R)
		{
			app->zoomUp(true);
		}
		else if (key == GLFW_KEY_F)
		{
			app->zoomDown(true);
		}
		else if (key == GLFW_KEY_SPACE)
		{
			app->homePos();
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
		else if (key == GLFW_KEY_R)
		{
			app->zoomUp(false);
		}
		else if (key == GLFW_KEY_F)
		{
			app->zoomDown(false);
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
	_thetaAng(0.0f),
	_zoomUp(false),
	_zoomDown(false),
	_distance(5.0f)
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
	float dt = glfwGetTime() - _oldTime;
	_oldTime = glfwGetTime();

	float speed = 1.0f;

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
	if (_zoomUp)
	{
		_distance += _distance * dt;
	}
	if (_zoomDown)
	{
		_distance -= _distance * dt;
	}

	_distance = glm::clamp(_distance, 0.5f, 50.0f);

	glm::vec3 pos = glm::vec3(glm::cos(_phiAng) * glm::cos(_thetaAng), glm::sin(_phiAng) * glm::cos(_thetaAng), glm::sin(_thetaAng)) * _distance;

	_viewMatrix = glm::lookAt(pos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Application::homePos()
{
	_phiAng = 0.0;
	_thetaAng = M_PI * 0.05;
	_distance = 20.0;

	glm::vec3 pos = glm::vec3(glm::cos(_phiAng) * glm::cos(_thetaAng), glm::sin(_phiAng) * glm::cos(_thetaAng), glm::sin(_thetaAng)) * _distance;

	_viewMatrix = glm::lookAt(pos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Application::makeSceneImplementation()
{
	//инициализация шейдера
	_commonShader.initialize();

	//загрузка текстур
	_worldTexId = loadTexture("images/earth_global.jpg");
	_brickTexId = loadTexture("images/brick.jpg");
	_grassTexId = loadTexture("images/grass.jpg");
	_specularTexId = loadTexture("images/specular.dds");
	_chessTexId = loadTextureWithMipmaps("images/chess.dds");
	_myTexId = makeCustomTexture();

	//загрузка 3д-моделей
	makeSphere(0.8f);
	makeCube(10.0f);
	makePlane();
	makeChessPlane();

	//инициализация параметров
	initData();
}

void Application::initData()
{
	//Инициализация матриц
	_viewMatrix = glm::lookAt(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	_projMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.f);

	//Инициализация значений переменных освщения
	_lightPos = glm::vec4(2.0f, 2.0f, 0.5f, 1.0f);
	_ambientColor = glm::vec3(0.2, 0.2, 0.2);
	_diffuseColor = glm::vec3(0.8, 0.8, 0.8);
	_specularColor = glm::vec3(0.5, 0.5, 0.5);

	//Инициализация сэмплера - объекта, который хранит параметры чтения из текстуры
	glGenSamplers(1, &_sampler);
	glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenSamplers(1, &_repeatSampler);	
	glSamplerParameteri(_repeatSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(_repeatSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameterf(_repeatSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
	glSamplerParameteri(_repeatSampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(_repeatSampler, GL_TEXTURE_WRAP_T, GL_REPEAT);	
}

void Application::drawImplementation()
{
	glUseProgram(_commonShader.getProgramId());

	_commonShader.setTime((float)glfwGetTime());
	_commonShader.setViewMatrix(_viewMatrix);
	_commonShader.setProjectionMatrix(_projMatrix);

	_commonShader.setLightPos(_lightPos);
	_commonShader.setAmbientColor(_ambientColor);
	_commonShader.setDiffuseColor(_diffuseColor);
	_commonShader.setSpecularColor(_specularColor);

	_commonShader.applyCommonUniforms();


	//====== Сфера ======

	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brickTexId);
	glBindSampler(0, _sampler);

	_commonShader.setDiffuseTexUnit(0); //текстурный юнит 0
	_commonShader.setModelMatrix(_sphereModelMatrix);
	_commonShader.setShininess(100.0f);

	_commonShader.applyMaterialUniforms();

	glBindVertexArray(_sphereVao); //Подключаем VertexArray для сферы
	glDrawArrays(GL_TRIANGLES, 0, _sphereNumTris * 3); //Рисуем сферу

	//====== Плоскость YZ ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brickTexId);
	glBindSampler(0, _sampler);

	_commonShader.setDiffuseTexUnit(0); //текстурный юнит 0
	_commonShader.setModelMatrix(_planeModelMatrix);
	_commonShader.setShininess(100.0f);

	_commonShader.applyMaterialUniforms();

	glBindVertexArray(_planeVao); //Подключаем VertexArray для плоскости
	glDrawArrays(GL_TRIANGLES, 0, 6); //Рисуем плоскость

#if 1
	//====== Плоскость XY ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _chessTexId);
	glBindSampler(0, _repeatSampler);

	_commonShader.setDiffuseTexUnit(0); //текстурный юнит 0
	_commonShader.setModelMatrix(glm::mat4(1.0));
	_commonShader.setShininess(100.0f);

	_commonShader.applyMaterialUniforms();

	glBindVertexArray(_chessVao); //Подключаем VertexArray для плоскости
	glDrawArrays(GL_TRIANGLES, 0, 6); //Рисуем плоскость
#endif

#if 1
	//====== Куб ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brickTexId);
	glBindSampler(0, _sampler);

	_commonShader.setDiffuseTexUnit(0); //текстурный юнит 0
	_commonShader.setModelMatrix(_cubeModelMatrix);
	_commonShader.setShininess(100.0f);

	_commonShader.applyMaterialUniforms();

	glBindVertexArray(_cubeVao); //Подключаем VertexArray для куба
	glDrawArrays(GL_TRIANGLES, 0, _cubeNumTris * 3); //Рисуем куб
#endif
}