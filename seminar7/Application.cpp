#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "Application.h"
#include "Texture.h"

int demoNum = 3;
//1 - Assimp and AntTweakBar demo
//2 - screen aligned quad
//3 - projective texture
//4 - copy texture
//5 - framebuffer

//Функция обратного вызова для обработки нажатий на клавиатуре. Определена в файле Navigation.cpp
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void windowSizeChangedCallback(GLFWwindow* window, int width, int height)
{
	Application* app = (Application*)glfwGetWindowUserPointer(window);
	app->setWindowSize(width, height);

	TwWindowSize(width, height);
}

void mouseButtonPressedCallback(GLFWwindow* window, int button, int action, int mods)
{
	TwEventMouseButtonGLFW(button, action);
}

void mouseCursosPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	TwEventMousePosGLFW(xpos, ypos);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	TwEventMouseWheelGLFW(xoffset);
}

void charCallback(GLFWwindow* window, unsigned int c)
{
	//TwEventCharGLFW
}

//==========================================================


Application::Application():
_oldTime(0.0f),
	_width(1280),
	_height(800),
	_lightTheta(0.7),
	_lightPhi(0.7),
	_lightR(10.0)
{
}

Application::~Application()
{
	TwTerminate();
	glfwTerminate();
}

void Application::initContext()
{
	if (!glfwInit())
	{
		std::cerr << "ERROR: could not start GLFW3\n";		
		exit(1);
	} 

	glfwWindowHint(GLFW_STENCIL_BITS, 8);

	_window = glfwCreateWindow(_width, _height, "Seminar 7", NULL, NULL);
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
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_POLYGON_OFFSET_FILL);	
}

void Application::initOthers()
{	
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(_width, _height);

	_bar = TwNewBar("TweakBar");
	TwDefine("GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.'");

	TwAddVarRW(_bar, "Color.z", TW_TYPE_FLOAT, &_diffuseColor.z, "min=0 max=1 step=0.01");
	TwAddVarRW(_bar, "Light phi", TW_TYPE_FLOAT, &_lightPhi, "step=0.01");
	TwAddVarRW(_bar, "Light theta", TW_TYPE_FLOAT, &_lightTheta, "step=0.01");

	glfwSetWindowSizeCallback(_window, windowSizeChangedCallback);
	glfwSetMouseButtonCallback(_window, mouseButtonPressedCallback);
	glfwSetCursorPosCallback(_window, mouseCursosPosCallback);
	glfwSetScrollCallback(_window, scrollCallback);
	glfwSetKeyCallback(_window, keyCallback);    
	glfwSetCharCallback(_window, charCallback);
}

void Application::setWindowSize(int width, int height)
{
	_width = width;
	_height = height;

	_mainCamera.setWindowSize(_width, _height);
}

void Application::makeScene()
{
	makeSceneImplementation();
}

void Application::run()
{
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

void Application::makeSceneImplementation()
{
	//инициализация шейдеров
	_commonMaterial.initialize();
	_skyBoxMaterial.initialize();
	_screenAlignedMaterial.initialize();

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
	_plane = Mesh::makeYZPlane(0.8f);
	_chess = Mesh::makeGroundPlane(100.0f, 100.0f);
	_cube = Mesh::makeCube(0.8f);
	_backgroundCube = Mesh::makeCube(10.0f);
	_bunny = Mesh::loadFromFile("models/bunny.obj");
	_screenQuad = Mesh::makeScreenAlignedQuad();

	//Инициализация значений переменных освщения
	_lightPos = glm::vec4(10.0f, 10.0f, 1.0f, 1.0f);
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

void Application::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);	

	glViewport(0, 0, _width, _height);

	//drawBackground(_mainCamera);
	drawScene(_mainCamera);

	TwDraw();

	glfwSwapBuffers(_window);
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

	glBindVertexArray(_backgroundCube.getVao()); //Подключаем VertexArray для куба
	glDrawArrays(GL_TRIANGLES, 0, _backgroundCube.getNumVertices()); //Рисуем куб

	glDepthMask(GL_TRUE);
}

void Application::drawScene(Camera& camera)
{
	//====== Остальные объекты ======	
	glUseProgram(_commonMaterial.getProgramId()); //Подключаем общий шейдер для всех объектов

	_commonMaterial.setTime((float)glfwGetTime());
	_commonMaterial.setViewMatrix(camera.getViewMatrix());
	_commonMaterial.setProjectionMatrix(camera.getProjMatrix());

	_lightPos = glm::vec4(glm::cos(_lightPhi) * glm::cos(_lightTheta) * _lightR, glm::sin(_lightPhi) * glm::cos(_lightTheta) * _lightR, glm::sin(_lightTheta) * _lightR, 1.0);

	_commonMaterial.setLightPos(_lightPos);
	_commonMaterial.setAmbientColor(_ambientColor);
	_commonMaterial.setDiffuseColor(_diffuseColor);
	_commonMaterial.setSpecularColor(_specularColor);

	_commonMaterial.applyCommonUniforms();

	if (demoNum == 1)
	{
		//====== Кролик ======
		glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
		glBindTexture(GL_TEXTURE_2D, _brickTexId);
		glBindSampler(0, _sampler);

		_commonMaterial.setDiffuseTexUnit(0); //текстурный юнит 0
		_commonMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		_commonMaterial.setShininess(100.0f);
		_commonMaterial.applyModelSpecificUniforms();	

		glBindVertexArray(_bunny.getVao()); //Подключаем VertexArray для плоскости
		glDrawArrays(GL_TRIANGLES, 0, _bunny.getNumVertices()); //Рисуем плоскость
	}


	if (demoNum != 1)
	{
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

		//====== Плоскость YZ ======
		glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
		glBindTexture(GL_TEXTURE_2D, _brickTexId);
		glBindSampler(0, _sampler);

		_commonMaterial.setDiffuseTexUnit(0); //текстурный юнит 0
		_commonMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		_commonMaterial.setShininess(100.0f);
		_commonMaterial.applyModelSpecificUniforms();

		glBindVertexArray(_cube.getVao()); //Подключаем VertexArray для куба
		glDrawArrays(GL_TRIANGLES, 0, _cube.getNumVertices()); //Рисуем куба
	}

	if (demoNum == 2)
	{
		glUseProgram(_screenAlignedMaterial.getProgramId());

		glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
		glBindTexture(GL_TEXTURE_2D, _brickTexId);
		glBindSampler(0, _sampler);

		_screenAlignedMaterial.setTexUnit(0); //текстурный юнит 0		
		_screenAlignedMaterial.applyModelSpecificUniforms();

		//glViewport(0, 0, 500, 500);

		glBindVertexArray(_screenQuad.getVao()); //Подключаем VertexArray для куба
		glDrawArrays(GL_TRIANGLES, 0, _screenQuad.getNumVertices()); //Рисуем куба
	}

	glBindSampler(0, 0);
	glUseProgram(0);
}