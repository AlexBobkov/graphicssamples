#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "Application.h"
#include "Texture.h"

int demoNum = 1;

bool hdr = true;
bool grayscale = false;
bool gamma = true;
bool secondLight = true;

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
	_lightR(10.0f),
	_ambientIntensity(0.2f),
	_diffuseIntensity(0.8f),
	_specularIntensity(0.5f),
	_exposure(1.0f)
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
	TwAddVarRW(_bar, "Light R", TW_TYPE_FLOAT, &_lightR, "min=5.0 max=100.0 step=0.1");
	TwAddVarRW(_bar, "Ambient intensity", TW_TYPE_FLOAT, &_ambientIntensity, "min=0.0 max=100.0 step=0.1");
	TwAddVarRW(_bar, "Diffuse intensity", TW_TYPE_FLOAT, &_diffuseIntensity, "min=0.0 max=100.0 step=0.1");
	TwAddVarRW(_bar, "Specular intensity", TW_TYPE_FLOAT, &_specularIntensity, "min=0.0 max=100.0 step=0.1");
	TwAddVarRW(_bar, "Exposure", TW_TYPE_FLOAT, &_exposure, "min=0.01 max=100.0 step=0.01");	


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

	glBindTexture(GL_TEXTURE_2D, _originImageTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, hdr ? GL_RGB16F : GL_RGB8, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);	

	glBindTexture(GL_TEXTURE_2D, _toneMappedImageTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, _brightImageTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _width / 2, _height / 2, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, _horizBlurImageTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _width / 2, _height / 2, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, _vertBlurImageTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _width / 2, _height / 2, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Application::makeScene()
{
	makeSceneImplementation();
}

void Application::makeSceneImplementation()
{
	//Инициализируем шейдеры
	_screenAlignedMaterial.initialize();
	_renderToShadowMaterial.initialize();
	_colorMaterial.initialize();
	_renderToGBufferMaterial.initialize();
	_deferredRenderingMaterial.initialize();

	_grayscaleEffect.setGrayscale(true);
	_grayscaleEffect.initialize();

	_gammaEffect.setGamma(true);
	_gammaEffect.initialize();

	_toneMappingEffect.initialize();

	_brightPass.setPassNumber(1);
	_brightPass.initialize();

	_horizBlurPass.setPassNumber(2);
	_horizBlurPass.initialize();

	_vertBlurPass.setPassNumber(3);
	_vertBlurPass.initialize();


	//Загружаем текстуры
	_worldTexId = Texture::loadTexture("images/earth_global.jpg");
	_brickTexId = Texture::loadTexture("images/brick.jpg");
	_grassTexId = Texture::loadTexture("images/grass.jpg");
	_colorTexId = Texture::makeCustomTexture();

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

	_light2.setLightPos(glm::vec3(0.0, 0.0, 1.0));
	_light2.setAmbientColor(glm::vec3(0.0, 0.0, 0.0));
	_light2.setDiffuseColor(glm::vec3(0.2, 0.2, 0.2));
	_light2.setSpecularColor(glm::vec3(0.0, 0.0, 0.0));
	
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

	//Сэмплер для чтения из карты теней
	glGenSamplers(1, &_depthSampler);
	glSamplerParameteri(_depthSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(_depthSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(_depthSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(_depthSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(_depthSampler, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glSamplerParameteri(_depthSampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);	

	initShadowFramebuffer(); //инициализация фреймбуфера для рендеринга в теневую карту
	initGBufferFramebuffer(); //инициализация фреймбуфера для рендеринга в G-буфер (текстура с нормалями, текстура с диффузным цветом, текстура с глубинами)
	initOriginImageFramebuffer();
	initToneMappingFramebuffer();
	initBloomFramebuffer();

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

void Application::initGBufferFramebuffer()
{
	//Создаем фреймбуфер
	glGenFramebuffers(1, &_GBufferFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, _GBufferFramebufferId);

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

void Application::initOriginImageFramebuffer()
{
	//Создаем фреймбуфер
	glGenFramebuffers(1, &_originImageFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, _originImageFramebufferId);

	GLint internalFormat = hdr ? GL_RGB16F : GL_RGB8;

	//Создаем текстуру, куда будет осуществляться рендеринг
	glGenTextures(1, &_originImageTexId);	
	glBindTexture(GL_TEXTURE_2D, _originImageTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _originImageTexId, 0);

	//Указываем куда именно мы будем рендерить		
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Failed to setup framebuffer\n";
		exit(1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Application::initToneMappingFramebuffer()
{
	//Создаем фреймбуфер
	glGenFramebuffers(1, &_toneMappingFramebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, _toneMappingFramebufferId);

	//Создаем текстуру, куда будет осуществляться рендеринг
	glGenTextures(1, &_toneMappedImageTexId);	
	glBindTexture(GL_TEXTURE_2D, _toneMappedImageTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _toneMappedImageTexId, 0);

	//Указываем куда именно мы будем рендерить		
	GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Failed to setup framebuffer\n";
		exit(1);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Application::initBloomFramebuffer()
{
	{
		//Создаем фреймбуфер
		glGenFramebuffers(1, &_brightFramebufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, _brightFramebufferId);

		//Создаем текстуру, куда будет осуществляться рендеринг
		glGenTextures(1, &_brightImageTexId);	
		glBindTexture(GL_TEXTURE_2D, _brightImageTexId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _width / 2, _height / 2, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _brightImageTexId, 0);

		//Указываем куда именно мы будем рендерить		
		GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, buffers);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Failed to setup framebuffer\n";
			exit(1);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	{
		//Создаем фреймбуфер
		glGenFramebuffers(1, &_horizBlurFramebufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, _horizBlurFramebufferId);

		//Создаем текстуру, куда будет осуществляться рендеринг
		glGenTextures(1, &_horizBlurImageTexId);	
		glBindTexture(GL_TEXTURE_2D, _horizBlurImageTexId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _width / 2, _height / 2, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _horizBlurImageTexId, 0);

		//Указываем куда именно мы будем рендерить		
		GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, buffers);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Failed to setup framebuffer\n";
			exit(1);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	{
		//Создаем фреймбуфер
		glGenFramebuffers(1, &_vertBlurFramebufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, _vertBlurFramebufferId);

		//Создаем текстуру, куда будет осуществляться рендеринг
		glGenTextures(1, &_vertBlurImageTexId);	
		glBindTexture(GL_TEXTURE_2D, _vertBlurImageTexId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _width / 2, _height / 2, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _vertBlurImageTexId, 0);

		//Указываем куда именно мы будем рендерить		
		GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, buffers);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Failed to setup framebuffer\n";
			exit(1);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
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
}

void Application::draw()
{
	renderToShadowMap(_lightCamera, _shadowFramebufferId);
	renderToGBuffer(_mainCamera, _GBufferFramebufferId);
	renderDeferred(_mainCamera, _lightCamera, _originImageFramebufferId);
	renderBloom();
	renderToneMapping(_toneMappingFramebufferId);

	//renderFinal(0, _originImageTexId);
	renderFinal(0, _toneMappedImageTexId);
	//renderFinal(0, _brightImageTexId);
	//renderFinal(0, _horizBlurImageTexId);
	//renderFinal(0, _vertBlurImageTexId);

	renderDebug(0, 0, 400, 400, _originImageTexId);

	TwDraw();

	glfwSwapBuffers(_window);
}

void Application::renderToShadowMap(Camera& lightCamera, GLuint fbId)
{
	//=========== Сначала подключаем фреймбуфер и рендерим в текстуру ==========
	glBindFramebuffer(GL_FRAMEBUFFER, fbId);

	glViewport(0, 0, _shadowMapWidth, _shadowMapHeight);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(_renderToShadowMaterial.getProgramId()); //Подключаем легкий шейдер для рендеринга в теневую карту

	_renderToShadowMaterial.setViewMatrix(lightCamera.getViewMatrix());
	_renderToShadowMaterial.setProjectionMatrix(lightCamera.getProjMatrix());
	_renderToShadowMaterial.applyCommonUniforms();

	glEnable(GL_CULL_FACE);    
	glFrontFace(GL_CW);
	glCullFace(GL_FRONT);

	//====== Сфера ======
	for (unsigned int i = 0; i < _positions.size(); i++)
	{
		_renderToShadowMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), _positions[i])); //считаем матрицу модели, используя координаты центра сферы
		_renderToShadowMaterial.applyModelSpecificUniforms();

		glBindVertexArray(_sphere.getVao()); //Подключаем VertexArray
		glDrawArrays(GL_TRIANGLES, 0, _sphere.getNumVertices()); //Рисуем
	}

	glDisable(GL_CULL_FACE);

	glBindVertexArray(_sphere.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _sphere.getNumVertices()); //Рисуем

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //Отключаем фреймбуфер
}

void Application::renderToGBuffer(Camera& mainCamera, GLuint fbId)
{
	//=========== Сначала подключаем фреймбуфер и рендерим в текстуру ==========
	glBindFramebuffer(GL_FRAMEBUFFER, fbId);

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

	glBindTexture(GL_TEXTURE_2D, _colorTexId);
	_renderToGBufferMaterial.setModelMatrix(glm::translate(glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f)), glm::vec3(0.0, -20.0, 0.0)));	
	_renderToGBufferMaterial.applyModelSpecificUniforms();

	glBindVertexArray(_teapot.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _teapot.getNumVertices()); //Рисуем

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

void Application::renderDeferred(Camera& mainCamera, Camera& lightCamera, GLuint fbId)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbId);

	glViewport(0, 0, _width, _height);
	glClearColor(199.0f / 255, 221.0f / 255, 235.0f / 255, 1); //blue color
	glClear(GL_COLOR_BUFFER_BIT);

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
	_deferredRenderingMaterial.setAddShadow(true);

	_deferredRenderingMaterial.applyCommonUniforms();
	_deferredRenderingMaterial.applyModelSpecificUniforms();


	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _normalsTexId);
	glBindSampler(0, _pixelPreciseSampler);

	glActiveTexture(GL_TEXTURE0 + 1);  //текстурный юнит 1
	glBindTexture(GL_TEXTURE_2D, _diffuseTexId);
	glBindSampler(1, _pixelPreciseSampler);

	glActiveTexture(GL_TEXTURE0 + 2);  //текстурный юнит 2
	glBindTexture(GL_TEXTURE_2D, _depthTexId);
	glBindSampler(2, _pixelPreciseSampler);

	glActiveTexture(GL_TEXTURE0 + 3);  //текстурный юнит 3
	glBindTexture(GL_TEXTURE_2D, _shadowMapTexId);
	glBindSampler(3, _depthSampler);
	

	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(_screenQuad.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _screenQuad.getNumVertices()); //Рисуем
	
	
	if (secondLight)
	{
		_deferredRenderingMaterial.setLightPos(_light2.getLightPos4());
		_deferredRenderingMaterial.setAmbientColor(_light2.getAmbientColor());
		_deferredRenderingMaterial.setDiffuseColor(_light2.getDiffuseColor());
		_deferredRenderingMaterial.setSpecularColor(_light2.getSpecularColor());
		_deferredRenderingMaterial.setAddShadow(false);
		_deferredRenderingMaterial.applyModelSpecificUniforms();

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		glBindVertexArray(_screenQuad.getVao()); //Подключаем VertexArray
		glDrawArrays(GL_TRIANGLES, 0, _screenQuad.getNumVertices()); //Рисуем

		glDisable(GL_BLEND);
	}

	glEnable(GL_DEPTH_TEST);

	glBindSampler(3, 0);
	glBindSampler(2, 0);
	glBindSampler(1, 0);
	glBindSampler(0, 0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Application::renderBloom()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _brightFramebufferId);

	glViewport(0, 0, _width / 2, _height / 2);


	glUseProgram(_brightPass.getProgramId());	
	_brightPass.setTexUnit(0); //текстурный юнит 0		
	_brightPass.applyModelSpecificUniforms();


	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _originImageTexId);
	glBindSampler(0, _sampler);

	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(_screenQuad.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _screenQuad.getNumVertices()); //Рисуем

	glEnable(GL_DEPTH_TEST);

	glBindSampler(0, 0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//=======================================================

	glBindFramebuffer(GL_FRAMEBUFFER, _horizBlurFramebufferId);

	glViewport(0, 0, _width / 2, _height / 2);


	glUseProgram(_horizBlurPass.getProgramId());	
	_horizBlurPass.setTexUnit(0); //текстурный юнит 0
	_horizBlurPass.setTexSize(glm::vec2(_width / 2, _height / 2));
	_horizBlurPass.applyModelSpecificUniforms();


	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brightImageTexId);
	glBindSampler(0, _sampler);

	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(_screenQuad.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _screenQuad.getNumVertices()); //Рисуем

	glEnable(GL_DEPTH_TEST);

	glBindSampler(0, 0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//=======================================================

	glBindFramebuffer(GL_FRAMEBUFFER, _vertBlurFramebufferId);

	glViewport(0, 0, _width / 2, _height / 2);


	glUseProgram(_vertBlurPass.getProgramId());	
	_vertBlurPass.setTexUnit(0); //текстурный юнит 0
	_vertBlurPass.setTexSize(glm::vec2(_width / 2, _height / 2));
	_vertBlurPass.applyModelSpecificUniforms();


	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _horizBlurImageTexId);
	glBindSampler(0, _sampler);

	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(_screenQuad.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _screenQuad.getNumVertices()); //Рисуем

	glEnable(GL_DEPTH_TEST);

	glBindSampler(0, 0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Application::renderToneMapping(GLuint fbId)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbId);

	glViewport(0, 0, _width, _height);


	glUseProgram(_toneMappingEffect.getProgramId());
	_toneMappingEffect.setExposure(_exposure);
	_toneMappingEffect.setTexUnit(0); //текстурный юнит 0		
	_toneMappingEffect.setBloomTexUnit(1); //текстурный юнит 1
	_toneMappingEffect.applyModelSpecificUniforms();


	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _originImageTexId);
	glBindSampler(0, _pixelPreciseSampler);

	glActiveTexture(GL_TEXTURE0 + 1);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _vertBlurImageTexId);
	glBindSampler(1, _sampler);


	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(_screenQuad.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _screenQuad.getNumVertices()); //Рисуем

	glEnable(GL_DEPTH_TEST);

	glBindSampler(0, 0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Application::renderFinal(GLuint fbId, GLuint texId)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbId);

	glViewport(0, 0, _width, _height);

	if (grayscale)
	{
		glUseProgram(_grayscaleEffect.getProgramId());
		_grayscaleEffect.setTexUnit(0); //текстурный юнит 0		
		_grayscaleEffect.applyModelSpecificUniforms();
	}
	else if (gamma)
	{
		glUseProgram(_gammaEffect.getProgramId());
		_gammaEffect.setTexUnit(0); //текстурный юнит 0		
		_gammaEffect.applyModelSpecificUniforms();
	}
	else
	{
		glUseProgram(_screenAlignedMaterial.getProgramId());
		_screenAlignedMaterial.setTexUnit(0); //текстурный юнит 0		
		_screenAlignedMaterial.applyModelSpecificUniforms();
	}

	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, texId);
	glBindSampler(0, _pixelPreciseSampler);

	glDisable(GL_DEPTH_TEST);

	glBindVertexArray(_screenQuad.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _screenQuad.getNumVertices()); //Рисуем

	glEnable(GL_DEPTH_TEST);

	glBindSampler(0, 0);
	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}