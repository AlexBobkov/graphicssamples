#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "Application.h"
#include "Texture.h"

int demoNum = 1;

int K = 10;

//Функция обратного вызова для обработки нажатий на клавиатуре. Определена в файле Navigation.cpp
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void getColorFromLinearPalette(float value, float& r, float& g, float& b);

void addVec3(std::vector<float>& vec, float x, float y, float z);

float frand()
{
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

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
	_lightTheta(0.7f),
	_lightPhi(0.7f),
	_lightR(10.0f),
	_ambientIntensity(0.2f),
	_diffuseIntensity(0.8f),
	_specularIntensity(0.5f),
	_fps(0.0)
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
	glewInit();

	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "OpenGL version supported: " << version << std::endl;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_POLYGON_OFFSET_FILL);	
}

void Application::initOthers()
{	
	//Настраиваем библиотеку AntTweakBar
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(_width, _height);

	_bar = TwNewBar("TweakBar");
	TwDefine("GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.'");
		
	TwAddVarRO(_bar, "FPS", TW_TYPE_FLOAT, &_fps, "");
	TwAddVarRW(_bar, "Light phi", TW_TYPE_FLOAT, &_lightPhi, "step=0.01");
	TwAddVarRW(_bar, "Light theta", TW_TYPE_FLOAT, &_lightTheta, "min=0.01 max=1.56 step=0.01");
	TwAddVarRW(_bar, "Light R", TW_TYPE_FLOAT, &_lightR, "min=5.0 max=100.0 step=0.1");
	TwAddVarRW(_bar, "Ambient intensity", TW_TYPE_FLOAT, &_ambientIntensity, "min=0.0 max=100.0 step=0.1");
	TwAddVarRW(_bar, "Diffuse intensity", TW_TYPE_FLOAT, &_diffuseIntensity, "min=0.0 max=100.0 step=0.1");
	TwAddVarRW(_bar, "Specular intensity", TW_TYPE_FLOAT, &_specularIntensity, "min=0.0 max=100.0 step=0.1");	


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

void Application::makeSceneImplementation()
{
	//Инициализируем шейдеры
	_screenAlignedMaterial.initialize();	
	_colorMaterial.initialize();
	_commonMaterial.initialize();

	//Загружаем текстуры
	_worldTexId = Texture::loadTexture("images/earth_global.jpg");
	_brickTexId = Texture::loadTexture("images/brick.jpg");
	_grassTexId = Texture::loadTexture("images/grass.jpg");
	_rotateTexId = Texture::loadTexture("images/rotate.png");
	_particleTexId = Texture::loadTexture("images/particle.png", true);
	_colorTexId = Texture::makeCustomTexture();	

	//инициализируем положения центров сфер
	float size = 10.0f;
	for (int i = 0; i < K; i++)
	{
		_positions.push_back(glm::vec3(frand() * size - 0.5 * size, frand() * size - 0.5 * size, 0.0));
	}

	//Загружаем 3д-модели
	_sphere = Mesh::makeSphere(0.8f);
	_plane = Mesh::makeYZPlane(0.8f);
	_ground = Mesh::makeGroundPlane(5.0f, 5.0f);
	_cube = Mesh::makeCube(0.8f);
	_backgroundCube = Mesh::makeCube(10.0f);
	_bunny = Mesh::loadFromFile("models/bunny.obj");
	_teapot = Mesh::loadFromFile("models/teapot.obj");
	_screenQuad = Mesh::makeScreenAlignedQuad();
	_sphereMarker = Mesh::makeSphere(0.1f);

	//Инициализацируем значения переменных освщения	
	_light.setAmbientColor(glm::vec3(_ambientIntensity, _ambientIntensity, _ambientIntensity));	
	_light.setDiffuseColor(glm::vec3(_diffuseIntensity, _diffuseIntensity, _diffuseIntensity));
	_light.setSpecularColor(glm::vec3(_specularIntensity, _specularIntensity, _specularIntensity));

	//_lightCamera.setProjMatrix(glm::perspective(glm::radians(90.0f), 1.0f, 5.0f, 20.f));
	_lightCamera.setProjMatrix(glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 30.f));

	//Инициализируем сэмплер - объект, который хранит параметры чтения из текстуры
	glGenSamplers(1, &_sampler);
	glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glGenSamplers(1, &_repeatSampler);	
	glSamplerParameteri(_repeatSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(_repeatSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameterf(_repeatSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
	glSamplerParameteri(_repeatSampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(_repeatSampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenSamplers(1, &_pixelPreciseSampler);
	glSamplerParameteri(_pixelPreciseSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(_pixelPreciseSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(_pixelPreciseSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(_pixelPreciseSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
	
	_light.setLightPos(glm::vec3(glm::cos(_lightPhi) * glm::cos(_lightTheta) * _lightR, glm::sin(_lightPhi) * glm::cos(_lightTheta) * _lightR, glm::sin(_lightTheta) * _lightR));
	_lightCamera.setCameraPos(_light.getLightPos());
	_lightCamera.setViewMatrix(glm::lookAt(_light.getLightPos(), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));

	_light.setAmbientColor(glm::vec3(_ambientIntensity, _ambientIntensity, _ambientIntensity));
	_light.setDiffuseColor(glm::vec3(_diffuseIntensity, _diffuseIntensity, _diffuseIntensity));
	_light.setSpecularColor(glm::vec3(_specularIntensity, _specularIntensity, _specularIntensity));

	_deltaTime = glfwGetTime() - _oldTime;
	_oldTime = glfwGetTime();
	_fps = 1 / _deltaTime;
}

void Application::draw()
{
	drawScene(_mainCamera);

	TwDraw();

	glfwSwapBuffers(_window);
}

void Application::drawScene(Camera& camera)
{
	glViewport(0, 0, _width, _height);
	glClearColor(199.0f / 255, 221.0f / 255, 235.0f / 255, 1); //blue color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glUseProgram(_commonMaterial.getProgramId());

	_commonMaterial.setTime((float)glfwGetTime());
	_commonMaterial.setViewMatrix(camera.getViewMatrix());
	_commonMaterial.setProjectionMatrix(camera.getProjMatrix());

	_commonMaterial.setLightPos(_light.getLightPos4());
	_commonMaterial.setAmbientColor(_light.getAmbientColor());
	_commonMaterial.setDiffuseColor(_light.getDiffuseColor());
	_commonMaterial.setSpecularColor(_light.getSpecularColor());

	_commonMaterial.applyCommonUniforms();

	//====== Сфера ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brickTexId);
	glBindSampler(0, _sampler);

	_commonMaterial.setDiffuseTexUnit(0); //текстурный юнит 0	
	_commonMaterial.setShininess(100.0f);
	

	for (unsigned int i = 0; i < _positions.size(); i++)
	{
		_commonMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), _positions[i])); //считаем матрицу модели, используя координаты центра сферы
		_commonMaterial.applyModelSpecificUniforms();

		glBindVertexArray(_sphere.getVao()); //Подключаем VertexArray
		glDrawArrays(GL_TRIANGLES, 0, _sphere.getNumVertices()); //Рисуем
	}


	////====== Плоскость земли ======
	//glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	//glBindTexture(GL_TEXTURE_2D, _brickTexId);
	//glBindSampler(0, _repeatSampler);

	//_commonMaterial.setDiffuseTexUnit(0); //текстурный юнит 0	
	//_commonMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));	
	//_commonMaterial.applyModelSpecificUniforms();

	//glBindVertexArray(_ground.getVao()); //Подключаем VertexArray
	//glDrawArrays(GL_TRIANGLES, 0, _ground.getNumVertices()); //Рисуем

	glUseProgram(0);
}
