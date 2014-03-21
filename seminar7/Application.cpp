#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "Application.h"
#include "Texture.h"

int demoNum = 1;
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
	_lightTheta(0.7f),
	_lightPhi(0.7f),
	_lightR(10.0f),
	_projTheta(0.0f),
	_projPhi(0.0f),
	_projR(10.0f)
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
	TwInit(TW_OPENGL, NULL);
	TwWindowSize(_width, _height);

	_bar = TwNewBar("TweakBar");
	TwDefine("GLOBAL help='This example shows how to integrate AntTweakBar with GLFW and OpenGL.'");

	TwAddVarRW(_bar, "Color.z", TW_TYPE_FLOAT, &_diffuseColor.z, "min=0 max=1 step=0.01");
	TwAddVarRW(_bar, "Light phi", TW_TYPE_FLOAT, &_lightPhi, "step=0.01");
	TwAddVarRW(_bar, "Light theta", TW_TYPE_FLOAT, &_lightTheta, "step=0.01");

	if (demoNum == 3)
	{
		TwAddVarRW(_bar, "Projector phi", TW_TYPE_FLOAT, &_projPhi, "step=0.01");
		TwAddVarRW(_bar, "Projector theta", TW_TYPE_FLOAT, &_projTheta, "step=0.01");
	}

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
	_projCamera.update();
}

void Application::makeSceneImplementation()
{
	//инициализация шейдеров
	_commonMaterial.initialize();
	_skyBoxMaterial.initialize();
	_screenAlignedMaterial.initialize();
	_projTextureMaterial.initialize();

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
	_ground = Mesh::makeGroundPlane(5.0f, 5.0f);
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
	glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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

	//инициализируем 2ю камеру для примера с 2мя камерами
	glm::vec3 secondCameraPos = glm::vec3(0.0f, 4.0f, 4.0);
	glm::mat4 secondViewMatrix = glm::lookAt(secondCameraPos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 secondProjMatrix = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.f);

	_secondCamera.setCameraPos(secondCameraPos);
	_secondCamera.setViewMatrix(secondViewMatrix);
	_secondCamera.setProjMatrix(secondProjMatrix);

	//инициализируем проектор
	glm::vec3 projCameraPos = glm::vec3(0.0f, 4.0f, 4.0);
	glm::mat4 projViewMatrix = glm::lookAt(projCameraPos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 projProjMatrix = glm::perspective(glm::radians(15.0f), 1.0f, 0.1f, 100.f);

	_projCamera.setViewMatrix(projViewMatrix);
	_projCamera.setProjMatrix(projProjMatrix);

	if (demoNum == 4)
	{
		glGenTextures(1, &_depthTexId);	
		glBindTexture(GL_TEXTURE_2D, _depthTexId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _width, _height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0);
	}

	if (demoNum == 5)
	{
		initFramebuffer();
	}
}

void Application::initFramebuffer()
{
	_fbWidth = 1024;
	_fbHeight = 1024;


	glGenFramebuffers(1, &_framebufferId);
	glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);


	//Создает текстуру, куда будет осуществляться рендеринг	
	glGenTextures(1, &_renderTexId);	
	glBindTexture(GL_TEXTURE_2D, _renderTexId);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _fbWidth, _fbHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _renderTexId, 0);


	//Создаем буфер глубины для фреймбуфера
	GLuint depthRenderBuffer;
	glGenRenderbuffers(1, &depthRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _fbWidth, _fbHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);


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

void Application::draw()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glViewport(0, 0, _width, _height);

	//drawBackground(_mainCamera);
	if (demoNum == 3)
	{
		drawProjScene(_mainCamera);
	}
	else if (demoNum == 5)
	{
		drawFramebufferDemo(_mainCamera, _secondCamera);
	}
	else
	{
		drawScene(_mainCamera);
	}

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
	glBindSampler(0, _sampler);

	_projTextureMaterial.setDiffuseTexUnit(0); //текстурный юнит 0
	_projTextureMaterial.setProjTexUnit(1); //текстурный юнит 1
	_projTextureMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	_projTextureMaterial.setShininess(100.0f);
	_projTextureMaterial.applyModelSpecificUniforms();

	glBindVertexArray(_ground.getVao()); //Подключаем VertexArray для куба
	glDrawArrays(GL_TRIANGLES, 0, _ground.getNumVertices()); //Рисуем куба

	glBindSampler(0, 0);
	glUseProgram(0);
}

void Application::drawFramebufferDemo(Camera& camera, Camera& fbCamera)
{	
	glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);

	glViewport(0, 0, _fbWidth, _fbHeight);

	glClearColor(1, 1, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	


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

	//====== Кролик ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brickTexId);
	glBindSampler(0, _sampler);

	_commonMaterial.setDiffuseTexUnit(0); //текстурный юнит 0
	_commonMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, sin((float)glfwGetTime()) - 1.0f)));
	_commonMaterial.setShininess(100.0f);
	_commonMaterial.applyModelSpecificUniforms();	

	glBindVertexArray(_bunny.getVao()); //Подключаем VertexArray для плоскости
	glDrawArrays(GL_TRIANGLES, 0, _bunny.getNumVertices()); //Рисуем плоскость

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

#if 0
	glUseProgram(_screenAlignedMaterial.getProgramId());

	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _renderTexId);
	glBindSampler(0, _sampler);

	_screenAlignedMaterial.setTexUnit(0); //текстурный юнит 0		
	_screenAlignedMaterial.applyModelSpecificUniforms();

	glViewport(0, 0, 500, 500);

	glBindVertexArray(_screenQuad.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _screenQuad.getNumVertices()); //Рисуем
#endif

	glBindSampler(0, 0);
	glUseProgram(0);
}