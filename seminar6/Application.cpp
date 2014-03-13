#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "Application.h"
#include "Texture.h"

int demoNum = 5;
//1 - простая кубическая текстура
//2 - 2 камеры
//3 - 2 плоскости (z-fighting)
//4 - face culling
//5 - blending

//Функция обратного вызова для обработки нажатий на клавиатуре. Определена в файле Navigation.cpp
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

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
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_POLYGON_OFFSET_FILL);	
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

void Application::update()
{
	_mainCamera.update();
}

void Application::draw()
{
	//Настройки размеров (если пользователь изменил размеры окна)
	int width, height;
	glfwGetFramebufferSize(_window, &width, &height);
	_mainCamera.setWindowSize(width, height);

	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	drawBackground(_mainCamera);
	drawScene(_mainCamera);

	if (demoNum == 2)
	{
		glViewport(0, 0, 200, 200);
		glClear(GL_DEPTH_BUFFER_BIT);	
		drawScene(_secondCamera);
	}

	glfwSwapBuffers(_window);
}

void Application::makeSceneImplementation()
{
	//инициализация шейдеров
	_commonMaterial.initialize();
	_skyBoxMaterial.initialize();

	//загрузка текстур
	_worldTexId = Texture::loadTexture("images/earth_global.jpg");
	_brickTexId = Texture::loadTexture("images/brick.jpg");
	_grassTexId = Texture::loadTexture("images/grass.jpg");
	_specularTexId = Texture::loadTexture("images/specular.dds");
	_chessTexId = Texture::loadTextureWithMipmaps("images/chess.dds");
	_myTexId = Texture::makeCustomTexture();
	_cubeTexId = Texture::loadCubeTexture("images/cube");
	_colorTexId = Texture::loadTexture("images/color.png");

	//загрузка 3д-моделей
	_sphere = Mesh::makeSphere(0.8f);
	_plane = Mesh::makePlane(0.8f);
	_chess = Mesh::makeChessPlane();
	_cube = Mesh::makeCube(10.0f);	

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

	//инициализируем 2ю камеру для примера с 2мя камерами
	glm::vec3 secondCameraPos = glm::vec3(0.0f, 4.0f, 4.0);
	glm::mat4 secondViewMatrix = glm::lookAt(secondCameraPos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 secondProjMatrix = glm::perspective(45.0f, 1.0f, 0.1f, 100.f);

	_secondCamera.setCameraPos(secondCameraPos);
	_secondCamera.setViewMatrix(secondViewMatrix);
	_secondCamera.setProjMatrix(secondProjMatrix);
}

void Application::drawBackground(Camera& camera)
{
	//====== Фоновый куб ======
	glUseProgram(_skyBoxMaterial.getProgramId()); //Подключаем шейдер для фонового куба

	_skyBoxMaterial.setCameraPos(camera.getCameraPos());
	_skyBoxMaterial.setViewMatrix(camera.getViewMatrix());
	_skyBoxMaterial.setProjectionMatrix(camera.getProjMatrix());	
	_skyBoxMaterial.applyCommonUniforms();	
	
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_CUBE_MAP, _cubeTexId);
	glBindSampler(0, _cubeSampler);

	_skyBoxMaterial.setTexUnit(0);  //текстурный юнит 0
	_skyBoxMaterial.applyModelSpecificUniforms();

	glDepthMask(GL_FALSE);

	glBindVertexArray(_cube.getVao()); //Подключаем VertexArray для куба
	glDrawArrays(GL_TRIANGLES, 0, _cube.getNumVertices()); //Рисуем куб

	glDepthMask(GL_TRUE);
}

void Application::drawScene(Camera& camera)
{
	//====== Остальные объекты ======	
	glUseProgram(_commonMaterial.getProgramId()); //Подключаем общий шейдер для всех объектов

	_commonMaterial.setTime((float)glfwGetTime());
	_commonMaterial.setViewMatrix(camera.getViewMatrix());
	_commonMaterial.setProjectionMatrix(camera.getProjMatrix());

	_commonMaterial.setLightPos(_lightPos);
	_commonMaterial.setAmbientColor(_ambientColor);
	_commonMaterial.setDiffuseColor(_diffuseColor);
	_commonMaterial.setSpecularColor(_specularColor);

	_commonMaterial.applyCommonUniforms();

	if (demoNum == 4 || demoNum == 5)
	{
		glEnable(GL_CULL_FACE);    
		glFrontFace(GL_CW);
		glCullFace(GL_BACK);
	}

	if (demoNum == 5)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	//====== Сфера ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brickTexId);
	glBindSampler(0, _sampler);

	_commonMaterial.setDiffuseTexUnit(0); //текстурный юнит 0
	_commonMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	_commonMaterial.setShininess(100.0f);
	_commonMaterial.applyModelSpecificUniforms();
	
	glBindVertexArray(_sphere.getVao()); //Подключаем VertexArray для сферы
	glDrawArrays(GL_TRIANGLES, 0, _sphere.getNumVertices()); //Рисуем сферу

#if 0
	glCullFace(GL_FRONT);
	glBindVertexArray(_sphere.getVao()); //Подключаем VertexArray для сферы
	glDrawArrays(GL_TRIANGLES, 0, _sphere.getNumVertices()); //Рисуем сферу
	glCullFace(GL_BACK);
	glBindVertexArray(_sphere.getVao()); //Подключаем VertexArray для сферы
	glDrawArrays(GL_TRIANGLES, 0, _sphere.getNumVertices()); //Рисуем сферу	
#endif

	//====== Плоскость YZ ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brickTexId);
	glBindSampler(0, _sampler);

	_commonMaterial.setDiffuseTexUnit(0); //текстурный юнит 0
	_commonMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	_commonMaterial.setShininess(100.0f);
	_commonMaterial.applyModelSpecificUniforms();

	glBindVertexArray(_plane.getVao()); //Подключаем VertexArray для плоскости
	glDrawArrays(GL_TRIANGLES, 0, _plane.getNumVertices()); //Рисуем плоскость

	if (demoNum == 3)
	{
		glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
		glBindTexture(GL_TEXTURE_2D, _worldTexId);
		glBindSampler(0, _sampler);

		_commonMaterial.setDiffuseTexUnit(0); //текстурный юнит 0
		_commonMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0001f, -1.0f, 0.0f)));
		_commonMaterial.setShininess(100.0f);
		_commonMaterial.applyModelSpecificUniforms();

		//glDisable(GL_DEPTH_TEST);
		//glPolygonOffset(-1.0f, -1.0f);

		glBindVertexArray(_plane.getVao()); //Подключаем VertexArray для плоскости
		glDrawArrays(GL_TRIANGLES, 0, _plane.getNumVertices()); //Рисуем плоскость

		//glPolygonOffset(0.0f, 0.0f);
		//glEnable(GL_DEPTH_TEST);
	}

	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
}