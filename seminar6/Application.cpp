#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "Application.h"

//Функция обратного вызова для обработки нажатий на клавиатуре
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Camera* camera = (Camera*)glfwGetWindowUserPointer(window);

	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if (key == GLFW_KEY_A)
		{
			camera->rotateLeft(true);
		}
		else if (key == GLFW_KEY_D)
		{
			camera->rotateRight(true);
		}
		else if (key == GLFW_KEY_W)
		{
			camera->rotateUp(true);
		}
		else if (key == GLFW_KEY_S)
		{
			camera->rotateDown(true);
		}
		else if (key == GLFW_KEY_R)
		{
			camera->zoomUp(true);
		}
		else if (key == GLFW_KEY_F)
		{
			camera->zoomDown(true);
		}
		else if (key == GLFW_KEY_SPACE)
		{
			camera->homePos();
		}
	}
	else if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_A)
		{
			camera->rotateLeft(false);
		}
		else if (key == GLFW_KEY_D)
		{
			camera->rotateRight(false);
		}
		else if (key == GLFW_KEY_W)
		{
			camera->rotateUp(false);
		}
		else if (key == GLFW_KEY_S)
		{
			camera->rotateDown(false);
		}
		else if (key == GLFW_KEY_R)
		{
			camera->zoomUp(false);
		}
		else if (key == GLFW_KEY_F)
		{
			camera->zoomDown(false);
		}
	}
}

//======================================

Application::Application():
_oldTime(0.0f)
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

	glfwSetWindowUserPointer(_window, &_mainCamera); //регистрируем указатель на данный объект, чтобы потом использовать его в функциях обратного вызова
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

	_mainCamera.setWindowSize(width, height);

	drawImplementation();

	glfwSwapBuffers(_window);	
}

void Application::update()
{
	_mainCamera.update();
}

void Application::makeSceneImplementation()
{
	//инициализация шейдеров
	_commonMaterial.initialize();
	_skyBoxMaterial.initialize();

	//загрузка текстур
	_worldTexId = loadTexture("images/earth_global.jpg");
	_brickTexId = loadTexture("images/brick.jpg");
	_grassTexId = loadTexture("images/grass.jpg");
	_specularTexId = loadTexture("images/specular.dds");
	_chessTexId = loadTextureWithMipmaps("images/chess.dds");
	_myTexId = makeCustomTexture();
	_cubeTexId = loadCubeTexture("images/cube");

	//загрузка 3д-моделей
	_sphereVao = makeSphere(0.8f, _sphereNumTris);
	_sphereModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		
	_planeVao = makePlane(_planeNumTris);
	_planeModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

	_chessVao = makeChessPlane(_chessNumTris);
	_cubeVao = makeCube(10.0f, _cubeNumTris);	

	//инициализация параметров
	initData();
}

void Application::initData()
{
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

	glGenSamplers(1, &_cubeSampler);	
	glSamplerParameteri(_cubeSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(_cubeSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(_cubeSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(_cubeSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
	glSamplerParameteri(_cubeSampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);	
}

void Application::drawImplementation()
{
	//====== Фоновый куб ======
	glUseProgram(_skyBoxMaterial.getProgramId()); //Подключаем шейдер для фонового куба

	_skyBoxMaterial.setCameraPos(_mainCamera.getCameraPos());
	_skyBoxMaterial.setViewMatrix(_mainCamera.getViewMatrix());
	_skyBoxMaterial.setProjectionMatrix(_mainCamera.getProjMatrix());	
	_skyBoxMaterial.applyCommonUniforms();	
	
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_CUBE_MAP, _cubeTexId);
	glBindSampler(0, _cubeSampler);

	_skyBoxMaterial.setTexUnit(0);  //текстурный юнит 0
	_skyBoxMaterial.applyModelSpecificUniforms();

	glDepthMask(GL_FALSE);

	glBindVertexArray(_cubeVao); //Подключаем VertexArray для куба
	glDrawArrays(GL_TRIANGLES, 0, _cubeNumTris * 3); //Рисуем куб

	glDepthMask(GL_TRUE);

	//====== Остальные объекты ======	
	glUseProgram(_commonMaterial.getProgramId()); //Подключаем общий шейдер для всех объектов

	_commonMaterial.setTime((float)glfwGetTime());
	_commonMaterial.setViewMatrix(_mainCamera.getViewMatrix());
	_commonMaterial.setProjectionMatrix(_mainCamera.getProjMatrix());

	_commonMaterial.setLightPos(_lightPos);
	_commonMaterial.setAmbientColor(_ambientColor);
	_commonMaterial.setDiffuseColor(_diffuseColor);
	_commonMaterial.setSpecularColor(_specularColor);

	_commonMaterial.applyCommonUniforms();


	//====== Сфера ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brickTexId);
	glBindSampler(0, _sampler);

	_commonMaterial.setDiffuseTexUnit(0); //текстурный юнит 0
	_commonMaterial.setModelMatrix(_sphereModelMatrix);
	_commonMaterial.setShininess(100.0f);

	_commonMaterial.applyModelSpecificUniforms();

	glBindVertexArray(_sphereVao); //Подключаем VertexArray для сферы
	glDrawArrays(GL_TRIANGLES, 0, _sphereNumTris * 3); //Рисуем сферу

	//====== Плоскость YZ ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brickTexId);
	glBindSampler(0, _sampler);

	_commonMaterial.setDiffuseTexUnit(0); //текстурный юнит 0
	_commonMaterial.setModelMatrix(_planeModelMatrix);
	_commonMaterial.setShininess(100.0f);

	_commonMaterial.applyModelSpecificUniforms();

	glBindVertexArray(_planeVao); //Подключаем VertexArray для плоскости
	glDrawArrays(GL_TRIANGLES, 0, 6); //Рисуем плоскость

#if 0
	//====== Плоскость XY ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _chessTexId);
	glBindSampler(0, _repeatSampler);

	_commonMaterial.setDiffuseTexUnit(0); //текстурный юнит 0
	_commonMaterial.setModelMatrix(glm::mat4(1.0));
	_commonMaterial.setShininess(100.0f);

	_commonMaterial.applyMaterialUniforms();

	glBindVertexArray(_chessVao); //Подключаем VertexArray для плоскости
	glDrawArrays(GL_TRIANGLES, 0, 6); //Рисуем плоскость
#endif
}