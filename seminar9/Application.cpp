#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "Application.h"
#include "Texture.h"

int demoNum = 1;

//Функция обратного вызова для обработки нажатий на клавиатуре. Определена в файле Navigation.cpp
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void getColorFromLinearPalette(float value, float& r, float& g, float& b);

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
	_lightR(10.0f)
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
		

	TwAddVarRW(_bar, "Light phi", TW_TYPE_FLOAT, &_lightPhi, "step=0.01");
	TwAddVarRW(_bar, "Light theta", TW_TYPE_FLOAT, &_lightTheta, "min=0.01 max=1.56 step=0.01");


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

	glBindTexture(GL_TEXTURE_2D, _normalsTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, _diffuseTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	
	glBindTexture(GL_TEXTURE_2D, _depthTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Application::makeScene()
{
	makeSceneImplementation();
}

void Application::makeSceneImplementation()
{
	//Инициализируем шейдеры
	_commonMaterial.initialize();
	_screenAlignedMaterial.initialize();
	_shadowMaterial.initialize();
	_renderToShadowMaterial.initialize();
	_colorMaterial.initialize();
	_renderToGBufferMaterial.initialize();
	_deferredRenderingMaterial.initialize();

	//Загружаем текстуры
	_worldTexId = Texture::loadTexture("images/earth_global.jpg");
	_brickTexId = Texture::loadTexture("images/brick.jpg");
	_grassTexId = Texture::loadTexture("images/grass.jpg");

	//Загружаем 3д-модели
	_sphere = Mesh::makeSphere(0.8f);
	_plane = Mesh::makeYZPlane(0.8f);
	_ground = Mesh::makeGroundPlane(5.0f, 5.0f);
	_cube = Mesh::makeCube(0.8f);
	_backgroundCube = Mesh::makeCube(10.0f);
	_bunny = Mesh::loadFromFile("models/bunny.obj");
	_screenQuad = Mesh::makeScreenAlignedQuad();
	_sphereMarker = Mesh::makeSphere(0.1f);

	//Инициализацируем значения переменных освщения	
	_light.setAmbientColor(glm::vec3(0.2, 0.2, 0.2));
	_light.setDiffuseColor(glm::vec3(0.8, 0.8, 0.8));
	_light.setSpecularColor(glm::vec3(0.5, 0.5, 0.5));

	_lightCamera.setProjMatrix(glm::perspective(glm::radians(90.0f), 1.0f, 5.0f, 20.f));
	
	//Инициализируем сэмплер - объект, который хранит параметры чтения из текстуры
	glGenSamplers(1, &_sampler);
	glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
	glGenSamplers(1, &_depthSampler);
	glSamplerParameteri(_depthSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(_depthSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(_depthSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(_depthSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(_depthSampler, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glSamplerParameteri(_depthSampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glGenSamplers(1, &_repeatSampler);	
	glSamplerParameteri(_repeatSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(_repeatSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameterf(_repeatSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
	glSamplerParameteri(_repeatSampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(_repeatSampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

	initShadowFramebuffer(); //инициализация фреймбуфера для рендеринга в теневую карту
	initDeferredRenderingFramebuffer(); //инициализация фреймбуфера для рендеринга в G-буфер (текстура с нормалями, текстура с диффузным цветом, текстура с глубинами)

	//инициализируем положения центров сфер
	float size = 10.0f;
	for (int i = 0; i < 10; i++)
	{
		_positions.push_back(glm::vec3(frand() * size - 0.5 * size, frand() * size - 0.5 * size, 0.0));
	}
}

void Application::initShadowFramebuffer()
{
	_shadowMapWidth = 1024;
	_shadowMapHeight = 1024;


	//Создаем фреймбуфер
	glGenFramebuffers(1, &_shadowFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, _shadowFramebufferId);


	////Создаем текстуру, куда будем впоследствии копировать буфер глубины
	glGenTextures(1, &_shadowMapTexId);	
	glBindTexture(GL_TEXTURE_2D, _shadowMapTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, _shadowMapWidth, _shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _shadowMapTexId, 0);


	//Указываем куда именно мы будем рендерить		
	GLenum buffers[] = { GL_NONE };
	glDrawBuffers(1, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Failed to setup framebuffer\n";
		exit(1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Application::initDeferredRenderingFramebuffer()
{
	//Создаем фреймбуфер
	glGenFramebuffers(1, &_deferredRenderingFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, _deferredRenderingFramebufferId);


	//Создаем текстуру, куда будет осуществляться рендеринг нормалей
	glGenTextures(1, &_normalsTexId);	
	glBindTexture(GL_TEXTURE_2D, _normalsTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _normalsTexId, 0);

	//Создаем текстуру, куда будет осуществляться рендеринг диффузного цвета
	glGenTextures(1, &_diffuseTexId);	
	glBindTexture(GL_TEXTURE_2D, _diffuseTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _diffuseTexId, 0);
	
	//Создаем текстуру, куда будем впоследствии копировать буфер глубины
	glGenTextures(1, &_depthTexId);	
	glBindTexture(GL_TEXTURE_2D, _depthTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthTexId, 0);


	//Указываем куда именно мы будем рендерить		
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Failed to setup framebuffer\n";
		exit(1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
}

void Application::draw()
{
	renderToShadowMap(_lightCamera);
	renderToGBuffer(_mainCamera);
	renderDeferred(_mainCamera, _lightCamera);

	renderDebug(0, 0, 400, 400, _shadowMapTexId);

	TwDraw();

	glfwSwapBuffers(_window);
}

void Application::renderToShadowMap(Camera& lightCamera)
{
	//=========== Сначала подключаем фреймбуфер и рендерим в текстуру ==========
	glBindFramebuffer(GL_FRAMEBUFFER, _shadowFramebufferId);

	glViewport(0, 0, _shadowMapWidth, _shadowMapHeight);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(_renderToShadowMaterial.getProgramId()); //Подключаем легкий шейдер для рендеринга в теневую карту

	_renderToShadowMaterial.setViewMatrix(lightCamera.getViewMatrix());
	_renderToShadowMaterial.setProjectionMatrix(lightCamera.getProjMatrix());
	_renderToShadowMaterial.applyCommonUniforms();


	//====== Сфера ======
	for (unsigned int i = 0; i < _positions.size(); i++)
	{
		_renderToShadowMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), _positions[i])); //считаем матрицу модели, используя координаты центра сферы
		_renderToShadowMaterial.applyModelSpecificUniforms();

		glBindVertexArray(_sphere.getVao()); //Подключаем VertexArray
		glDrawArrays(GL_TRIANGLES, 0, _sphere.getNumVertices()); //Рисуем
	}

	glBindVertexArray(_sphere.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _sphere.getNumVertices()); //Рисуем

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //Отключаем фреймбуфер
}

void Application::renderToGBuffer(Camera& mainCamera)
{
	//=========== Сначала подключаем фреймбуфер и рендерим в текстуру ==========
	glBindFramebuffer(GL_FRAMEBUFFER, _deferredRenderingFramebufferId);

	glViewport(0, 0, _width, _height);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	


	glUseProgram(_renderToGBufferMaterial.getProgramId()); //Подключаем шейдер, который рендерит в 3 текстуры: текстуру с нормалями, с глубинами, с диффузным цветом

	_renderToGBufferMaterial.setViewMatrix(mainCamera.getViewMatrix());
	_renderToGBufferMaterial.setProjectionMatrix(mainCamera.getProjMatrix());	
	_renderToGBufferMaterial.applyCommonUniforms();

	//====== Сфера ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brickTexId);
	glBindSampler(0, _sampler);

	_renderToGBufferMaterial.setDiffuseTexUnit(0); //текстурный юнит 0	

	for (unsigned int i = 0; i < _positions.size(); i++)
	{
		_renderToGBufferMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), _positions[i])); //считаем матрицу модели, используя координаты центра сферы
		_renderToGBufferMaterial.applyModelSpecificUniforms();

		glBindVertexArray(_sphere.getVao()); //Подключаем VertexArray
		glDrawArrays(GL_TRIANGLES, 0, _sphere.getNumVertices()); //Рисуем
	}

	//====== Плоскость земли ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brickTexId);
	glBindSampler(0, _repeatSampler);

	_renderToGBufferMaterial.setDiffuseTexUnit(0); //текстурный юнит 0	
	_renderToGBufferMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));	
	_renderToGBufferMaterial.applyModelSpecificUniforms();

	glBindVertexArray(_ground.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _ground.getNumVertices()); //Рисуем

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Application::renderDebug(int x, int y, int width, int height, GLuint texId)
{
	glViewport(x, y, width, height);

	//====== В целях отладки рисуем на экран прямоугольник с теневой картой
	glUseProgram(_screenAlignedMaterial.getProgramId());

	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, texId);
	glBindSampler(0, _sampler);

	_screenAlignedMaterial.setTexUnit(0); //текстурный юнит 0		
	_screenAlignedMaterial.applyModelSpecificUniforms();

	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(_screenQuad.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _screenQuad.getNumVertices()); //Рисуем

	glEnable(GL_DEPTH_TEST);

	glBindSampler(0, 0);
	glUseProgram(0);
}

void Application::renderDeferred(Camera& mainCamera, Camera& lightCamera)
{
	glViewport(0, 0, _width, _height);
	glClearColor(199.0f / 255, 221.0f / 255, 235.0f / 255, 1); //blue color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//====== Рисуем на экран прямоугольник
	glUseProgram(_deferredRenderingMaterial.getProgramId()); //шейдер для отложенного освещения, который принимает на вход 3 текстуры: текстуру с нормалями, с глубинами, с диффузным цветом
	
	_deferredRenderingMaterial.setViewMatrix(mainCamera.getViewMatrix());

	_deferredRenderingMaterial.setViewMatrixInverse(glm::inverse(mainCamera.getViewMatrix()));
	_deferredRenderingMaterial.setProjMatrixInverse(glm::inverse(mainCamera.getProjMatrix()));

	_deferredRenderingMaterial.setLightViewMatrix(lightCamera.getViewMatrix());
	_deferredRenderingMaterial.setLightProjectionMatrix(lightCamera.getProjMatrix());

	_deferredRenderingMaterial.setNormalsTexUnit(0);
	_deferredRenderingMaterial.setDiffuseTexUnit(1);
	_deferredRenderingMaterial.setDepthTexUnit(2);
	_deferredRenderingMaterial.setShadowTexUnit(3);

	_deferredRenderingMaterial.setLightPos(_light.getLightPos4());
	_deferredRenderingMaterial.setAmbientColor(_light.getAmbientColor());
	_deferredRenderingMaterial.setDiffuseColor(_light.getDiffuseColor());
	_deferredRenderingMaterial.setSpecularColor(_light.getSpecularColor());

	_deferredRenderingMaterial.applyCommonUniforms();
	_deferredRenderingMaterial.applyModelSpecificUniforms();


	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _normalsTexId);
	glBindSampler(0, _sampler);

	glActiveTexture(GL_TEXTURE0 + 1);  //текстурный юнит 1
	glBindTexture(GL_TEXTURE_2D, _diffuseTexId);
	glBindSampler(1, _sampler);

	glActiveTexture(GL_TEXTURE0 + 2);  //текстурный юнит 2
	glBindTexture(GL_TEXTURE_2D, _depthTexId);
	glBindSampler(2, _sampler);

	glActiveTexture(GL_TEXTURE0 + 3);  //текстурный юнит 3
	glBindTexture(GL_TEXTURE_2D, _shadowMapTexId);
	//glBindSampler(3, _depthSampler);
	glBindSampler(3, _sampler);
	

	glBindVertexArray(_screenQuad.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _screenQuad.getNumVertices()); //Рисуем


	glBindSampler(0, 0);
	glUseProgram(0);
}
