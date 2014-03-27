#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "Application.h"
#include "Texture.h"

int demoNum = 1;
//1 - shadow map
//2 - advanced shadow map
//3 - many objects
//4 - 

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
	_lightTheta(0.7f),
	_lightPhi(0.7f),
	_lightR(7.0f)
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

	TwAddVarRW(_bar, "Color.z", TW_TYPE_FLOAT, &_diffuseColor.z, "min=0 max=1 step=0.01");
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
	_projTextureMaterial.initialize();
	_renderToShadowMaterial.initialize();

	//Загружаем текстуры
	_worldTexId = Texture::loadTexture("images/earth_global.jpg");
	_brickTexId = Texture::loadTexture("images/brick.jpg");
	_grassTexId = Texture::loadTexture("images/grass.jpg");
	_specularTexId = Texture::loadTexture("images/specular.dds");
	_chessTexId = Texture::loadTextureWithMipmaps("images/chess.dds");
	_myTexId = Texture::makeCustomTexture();
	_cubeTexId = Texture::loadCubeTexture("images/cube");
	_colorTexId = Texture::loadTexture("images/color.png");

	//Загружаем 3д-модели
	_sphere = Mesh::makeSphere(0.8f);
	_plane = Mesh::makeYZPlane(0.8f);
	_ground = Mesh::makeGroundPlane(5.0f, 5.0f);
	_cube = Mesh::makeCube(0.8f);
	_backgroundCube = Mesh::makeCube(10.0f);
	_bunny = Mesh::loadFromFile("models/bunny.obj");
	_screenQuad = Mesh::makeScreenAlignedQuad();

	//Инициализацируем значения переменных освщения
	_lightPos = glm::vec4(10.0f, 10.0f, 1.0f, 1.0f);
	_ambientColor = glm::vec3(0.2, 0.2, 0.2);
	_diffuseColor = glm::vec3(0.8, 0.8, 0.8);
	_specularColor = glm::vec3(0.5, 0.5, 0.5);

	//Инициализируем сэмплер - объект, который хранит параметры чтения из текстуры
	glGenSamplers(1, &_sampler);
	glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glGenSamplers(1, &_repeatSampler);	
	glSamplerParameteri(_repeatSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(_repeatSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameterf(_repeatSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
	glSamplerParameteri(_repeatSampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(_repeatSampler, GL_TEXTURE_WRAP_T, GL_REPEAT);	

	glGenSamplers(1, &_cubeSampler);	
	glSamplerParameteri(_cubeSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(_cubeSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(_cubeSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(_cubeSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
	glSamplerParameteri(_cubeSampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//Инициализируем 2ю камеру для примера с 2мя камерами
	_lightCamera.setProjMatrix(glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 20.f));

	initShadowFramebuffer();
}

void Application::initShadowFramebuffer()
{
	_fbWidth = 1024;
	_fbHeight = 1024;


	//Создаем фреймбуфер
	glGenFramebuffers(1, &_framebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);


	////Создаем текстуру, куда будем впоследствии копировать буфер глубины
	glGenTextures(1, &_depthTexId);	
	glBindTexture(GL_TEXTURE_2D, _depthTexId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, _fbWidth, _fbHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthTexId, 0);


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
	
	_lightPos = glm::vec4(glm::cos(_lightPhi) * glm::cos(_lightTheta) * _lightR, glm::sin(_lightPhi) * glm::cos(_lightTheta) * _lightR, glm::sin(_lightTheta) * _lightR, 1.0);
	_lightCamera.setCameraPos(glm::vec3(_lightPos.x, _lightPos.y, _lightPos.z));
	_lightCamera.setViewMatrix(glm::lookAt(glm::vec3(_lightPos.x, _lightPos.y, _lightPos.z), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
}

void Application::draw()
{
	drawToShadowMap(_lightCamera);
	drawSceneWithShadow(_mainCamera, _lightCamera);	

	TwDraw();

	glfwSwapBuffers(_window);
}

void Application::drawToShadowMap(Camera& lightCamera)
{
	//=========== Сначала подключаем фреймбуфер и рендерим в текстуру ==========
	glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);

	glViewport(0, 0, _fbWidth, _fbHeight);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	glUseProgram(_renderToShadowMaterial.getProgramId()); //Подключаем общий шейдер для всех объектов

	_renderToShadowMaterial.setViewMatrix(lightCamera.getViewMatrix());
	_renderToShadowMaterial.setProjectionMatrix(lightCamera.getProjMatrix());
	_renderToShadowMaterial.applyCommonUniforms();

	//====== Кролик ======
	_renderToShadowMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, sin((float)glfwGetTime()))));	
	_renderToShadowMaterial.applyModelSpecificUniforms();	

	glBindVertexArray(_bunny.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _bunny.getNumVertices()); //Рисуем

	//====== Сфера ======
	_renderToShadowMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	_renderToShadowMaterial.applyModelSpecificUniforms();

	glBindVertexArray(_sphere.getVao()); //Подключаем VertexArray для сферы
	glDrawArrays(GL_TRIANGLES, 0, _sphere.getNumVertices()); //Рисуем сферу

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Application::drawSceneWithShadow(Camera& mainCamera, Camera& lightCamera)
{
	glViewport(0, 0, _width, _height);
	glClearColor(199.0 / 255.0, 221.0 / 255.0, 235.0 / 255.0, 1); //blue color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	


	//====== Рисуем на экран тех же самых кролика и сферу
	glUseProgram(_commonMaterial.getProgramId()); //Подключаем общий шейдер для всех объектов

	_commonMaterial.setTime((float)glfwGetTime());
	_commonMaterial.setViewMatrix(mainCamera.getViewMatrix());
	_commonMaterial.setProjectionMatrix(mainCamera.getProjMatrix());

	_commonMaterial.setLightPos(_lightPos);
	_commonMaterial.setAmbientColor(_ambientColor);
	_commonMaterial.setDiffuseColor(_diffuseColor);
	_commonMaterial.setSpecularColor(_specularColor);

	_commonMaterial.applyCommonUniforms();


	//====== Кролик ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brickTexId);
	glBindSampler(0, _sampler);

	_commonMaterial.setDiffuseTexUnit(0); //текстурный юнит 0
	_commonMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, sin((float)glfwGetTime()))));
	_commonMaterial.setShininess(100.0f);
	_commonMaterial.applyModelSpecificUniforms();	

	glBindVertexArray(_bunny.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _bunny.getNumVertices()); //Рисуем

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

	//====== Плоскость земли ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brickTexId);
	glBindSampler(0, _repeatSampler);

	_commonMaterial.setDiffuseTexUnit(0); //текстурный юнит 0	
	_commonMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	_commonMaterial.setShininess(100.0f);
	_commonMaterial.applyModelSpecificUniforms();

	glBindVertexArray(_ground.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _ground.getNumVertices()); //Рисуем


	//====== В целях отладки рисуем на экран прямоугольник с теневой картой
	glUseProgram(_screenAlignedMaterial.getProgramId());

	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _depthTexId);
	glBindSampler(0, _sampler);

	_screenAlignedMaterial.setTexUnit(0); //текстурный юнит 0		
	_screenAlignedMaterial.applyModelSpecificUniforms();

	glViewport(0, 0, 400, 400);

	glBindVertexArray(_screenQuad.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _screenQuad.getNumVertices()); //Рисуем

	glBindSampler(0, 0);
	glUseProgram(0);
}

/*
void Application::drawMultiObjectScene(Camera& camera)
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

		glBindVertexArray(_bunny.getVao()); //Подключаем VertexArray
		glDrawArrays(GL_TRIANGLES, 0, _bunny.getNumVertices()); //Рисуем
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

		//====== Куб ======
		glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
		glBindTexture(GL_TEXTURE_2D, _brickTexId);
		glBindSampler(0, _sampler);

		_commonMaterial.setDiffuseTexUnit(0); //текстурный юнит 0
		_commonMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		_commonMaterial.setShininess(100.0f);
		_commonMaterial.applyModelSpecificUniforms();

		glBindVertexArray(_cube.getVao()); //Подключаем VertexArray для куба
		glDrawArrays(GL_TRIANGLES, 0, _cube.getNumVertices()); //Рисуем куб
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

		glBindVertexArray(_screenQuad.getVao()); //Подключаем VertexArray
		glDrawArrays(GL_TRIANGLES, 0, _screenQuad.getNumVertices()); //Рисуем
	}

	if (demoNum == 4)
	{
		glBindTexture(GL_TEXTURE_2D, _depthTexId);
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, _width, _height, 0);

		glUseProgram(_screenAlignedMaterial.getProgramId());

		glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
		glBindTexture(GL_TEXTURE_2D, _depthTexId);
		glBindSampler(0, _sampler);

		_screenAlignedMaterial.setTexUnit(0); //текстурный юнит 0		
		_screenAlignedMaterial.applyModelSpecificUniforms();

		glViewport(0, 0, 500, 500);

		glBindVertexArray(_screenQuad.getVao()); //Подключаем VertexArray
		glDrawArrays(GL_TRIANGLES, 0, _screenQuad.getNumVertices()); //Рисуем
	}

	glBindSampler(0, 0);
	glUseProgram(0);
}
*/

/*
void Application::drawProjScene(Camera& camera)
{
	//====== Остальные объекты ======	
	glUseProgram(_projTextureMaterial.getProgramId()); //Подключаем общий шейдер для всех объектов

	_projTextureMaterial.setTime((float)glfwGetTime());
	_projTextureMaterial.setViewMatrix(camera.getViewMatrix());
	_projTextureMaterial.setProjectionMatrix(camera.getProjMatrix());

	glm::vec3 projPos = glm::vec3(glm::cos(_projPhi) * glm::cos(_projTheta) * _projR, glm::sin(_projPhi) * glm::cos(_projTheta) * _projR, glm::sin(_projTheta) * _projR);
	glm::mat4 projViewMatrix = glm::lookAt(projPos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));		
	_projCamera.setViewMatrix(projViewMatrix);

	_projTextureMaterial.setProjViewMatrix(_projCamera.getViewMatrix());
	_projTextureMaterial.setProjProjectionMatrix(_projCamera.getProjMatrix());

	_lightPos = glm::vec4(glm::cos(_lightPhi) * glm::cos(_lightTheta) * _lightR, glm::sin(_lightPhi) * glm::cos(_lightTheta) * _lightR, glm::sin(_lightTheta) * _lightR, 1.0);

	_projTextureMaterial.setLightPos(_lightPos);
	_projTextureMaterial.setAmbientColor(_ambientColor);
	_projTextureMaterial.setDiffuseColor(_diffuseColor);
	_projTextureMaterial.setSpecularColor(_specularColor);

	_projTextureMaterial.applyCommonUniforms();

	glActiveTexture(GL_TEXTURE0 + 1);  //текстурный юнит 1
	glBindTexture(GL_TEXTURE_2D, _worldTexId);
	glBindSampler(1, _sampler);

	//====== Сфера ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brickTexId);
	glBindSampler(0, _sampler);

	_projTextureMaterial.setDiffuseTexUnit(0); //текстурный юнит 0
	_projTextureMaterial.setProjTexUnit(1); //текстурный юнит 1
	_projTextureMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	_projTextureMaterial.setShininess(100.0f);
	_projTextureMaterial.applyModelSpecificUniforms();

	glBindVertexArray(_sphere.getVao()); //Подключаем VertexArray для сферы
	glDrawArrays(GL_TRIANGLES, 0, _sphere.getNumVertices()); //Рисуем сферу

	//====== Куб ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brickTexId);
	glBindSampler(0, _sampler);

	_projTextureMaterial.setDiffuseTexUnit(0); //текстурный юнит 0
	_projTextureMaterial.setProjTexUnit(1); //текстурный юнит 1
	_projTextureMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	_projTextureMaterial.setShininess(100.0f);
	_projTextureMaterial.applyModelSpecificUniforms();

	glBindVertexArray(_cube.getVao()); //Подключаем VertexArray для куба
	glDrawArrays(GL_TRIANGLES, 0, _cube.getNumVertices()); //Рисуем куба

	//====== Плоскость земли ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brickTexId);
	glBindSampler(0, _repeatSampler);

	_projTextureMaterial.setDiffuseTexUnit(0); //текстурный юнит 0
	_projTextureMaterial.setProjTexUnit(1); //текстурный юнит 1
	_projTextureMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	_projTextureMaterial.setShininess(100.0f);
	_projTextureMaterial.applyModelSpecificUniforms();

	glBindVertexArray(_ground.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _ground.getNumVertices()); //Рисуем

	glBindSampler(0, 0);
	glUseProgram(0);
}

void Application::drawFramebufferDemo(Camera& camera, Camera& fbCamera)
{	
	//=========== Сначала подключаем фреймбуфер и рендерим в текстуру ==========
	glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);

	glViewport(0, 0, _fbWidth, _fbHeight);

	glClearColor(1, 1, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	


	glUseProgram(_commonMaterial.getProgramId()); //Подключаем общий шейдер для всех объектов

	_commonMaterial.setTime((float)glfwGetTime());
	_commonMaterial.setViewMatrix(fbCamera.getViewMatrix());
	_commonMaterial.setProjectionMatrix(fbCamera.getProjMatrix());

	_lightPos = glm::vec4(glm::cos(_lightPhi) * glm::cos(_lightTheta) * _lightR, glm::sin(_lightPhi) * glm::cos(_lightTheta) * _lightR, glm::sin(_lightTheta) * _lightR, 1.0);

	_commonMaterial.setLightPos(_lightPos);
	_commonMaterial.setAmbientColor(_ambientColor);
	_commonMaterial.setDiffuseColor(_diffuseColor);
	_commonMaterial.setSpecularColor(_specularColor);

	_commonMaterial.applyCommonUniforms();

	//====== Кролик ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brickTexId);
	glBindSampler(0, _sampler);

	_commonMaterial.setDiffuseTexUnit(0); //текстурный юнит 0
	_commonMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, sin((float)glfwGetTime()) - 1.0f)));
	_commonMaterial.setShininess(100.0f);
	_commonMaterial.applyModelSpecificUniforms();	

	glBindVertexArray(_bunny.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _bunny.getNumVertices()); //Рисуем

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

	glUseProgram(0);

	//==================================================================================
	//=========== Теперь отключаем фреймбуфер и рендерим на экран ==========

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, _width, _height);


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


	//====== Сфера ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _renderTexId);
	glBindSampler(0, _sampler);

	_commonMaterial.setDiffuseTexUnit(0); //текстурный юнит 0
	_commonMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
	_commonMaterial.setShininess(100.0f);
	_commonMaterial.applyModelSpecificUniforms();

	glBindVertexArray(_sphere.getVao()); //Подключаем VertexArray для сферы
	glDrawArrays(GL_TRIANGLES, 0, _sphere.getNumVertices()); //Рисуем сферу

	//====== Куб ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _renderTexId);
	glBindSampler(0, _sampler);

	_commonMaterial.setDiffuseTexUnit(0); //текстурный юнит 0
	_commonMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	_commonMaterial.setShininess(100.0f);
	_commonMaterial.applyModelSpecificUniforms();

	glBindVertexArray(_cube.getVao()); //Подключаем VertexArray для куба
	glDrawArrays(GL_TRIANGLES, 0, _cube.getNumVertices()); //Рисуем куба


	glBindSampler(0, 0);
	glUseProgram(0);
}*/