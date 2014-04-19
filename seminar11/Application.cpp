#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "Application.h"
#include "Texture.h"

#ifdef OCULUS_FOUND
using namespace OVR;
#endif

int demoNum = 1;
//1 - simple stereo without Oculus params
//2 - oculus simple stereo without distortion
//3 - oculus distortion

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
	_fps(0.0),
	_iod(0.06),
	_sd(0.4)
{

}

Application::~Application()
{
	destroyOVR();

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

	_window = glfwCreateWindow(_width, _height, "Seminar 11", glfwGetPrimaryMonitor(), NULL);
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

void Application::initOVR()
{
#ifdef OCULUS_FOUND
	System::Init();

	_pFusionResult = new SensorFusion();
	_pManager = *DeviceManager::Create();

	_pHMD = *_pManager->EnumerateDevices<HMDDevice>().CreateDevice();

	if (_pHMD)
	{
		_infoLoaded = _pHMD->GetDeviceInfo(&_info);

		_pSensor = *_pHMD->GetSensor();
	}

	if (_pSensor)
	{
		_pFusionResult->AttachToSensor(_pSensor);
	}

	std::cout << "----- Oculus Console -----" << std::endl;

	if (_pHMD)
	{
		std::cout << " [x] HMD Found" << std::endl;
	}
	else
	{
		std::cout << " [ ] HMD Not Found" << std::endl;
	}

	if (_pSensor)
	{
		std::cout << " [x] Sensor Found" << std::endl;
	}
	else
	{
		std::cout << " [ ] Sensor Not Found" << std::endl;
	}

	std::cout << "--------------------------" << std::endl;

	if (_infoLoaded)
	{
		std::cout << " DisplayDeviceName: " << _info.DisplayDeviceName << std::endl;
		std::cout << " ProductName: " << _info.ProductName << std::endl;
		std::cout << " Manufacturer: " << _info.Manufacturer << std::endl;
		std::cout << " Version: " << _info.Version << std::endl;
		std::cout << " HResolution: " << _info.HResolution<< std::endl;
		std::cout << " VResolution: " << _info.VResolution<< std::endl;
		std::cout << " HScreenSize: " << _info.HScreenSize<< std::endl;
		std::cout << " VScreenSize: " << _info.VScreenSize<< std::endl;
		std::cout << " VScreenCenter: " << _info.VScreenCenter<< std::endl;
		std::cout << " EyeToScreenDistance: " << _info.EyeToScreenDistance << std::endl;
		std::cout << " LensSeparationDistance: " << _info.LensSeparationDistance << std::endl;
		std::cout << " InterpupillaryDistance: " << _info.InterpupillaryDistance << std::endl;
		std::cout << " DistortionK[0]: " << _info.DistortionK[0] << std::endl;
		std::cout << " DistortionK[1]: " << _info.DistortionK[1] << std::endl;
		std::cout << " DistortionK[2]: " << _info.DistortionK[2] << std::endl;
		std::cout << "--------------------------" << std::endl;
	}
#endif
}

void Application::destroyOVR()
{
#ifdef OCULUS_FOUND
	_pSensor.Clear();
	_pHMD.Clear();
	_pManager.Clear();

	delete _pFusionResult;

	System::Destroy();
#endif
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
	_oculusDistortionShader.initialize();	
	_colorMaterial.initialize();
	_commonMaterial.initialize();

	//Загружаем текстуры
	_worldTexId = Texture::loadTexture("images/earth_global.jpg");
	_brickTexId = Texture::loadTexture("images/brick.jpg");
	_grassTexId = Texture::loadTexture("images/grass.jpg");
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

#ifdef OCULUS_FOUND
	_scaleFactor = 1.25;
	{
		float lensShift = _info.HScreenSize * 0.25f - _info.LensSeparationDistance * 0.5f;
		float lensViewportShift = 4.0f * lensShift / _info.HScreenSize;
		float fitRadius = fabs(-1 - lensViewportShift);
		float rsq = fitRadius * fitRadius;
		
		_scaleFactor = (_info.DistortionK[0] + _info.DistortionK[1] * rsq + _info.DistortionK[2] * rsq * rsq + _info.DistortionK[3] * rsq * rsq * rsq);

		std::cout << "Scale factor " << _scaleFactor << std::endl;
	}

	_aspectRatio = _width / 2.0f / _height;
	float fov = 2.0 * atan(_info.VScreenSize * _scaleFactor / (2.0 * _info.EyeToScreenDistance));
	_mainCamera.setProjMatrix(glm::perspective(fov, _aspectRatio, 1.0f, 500.f));

	std::cout << "aspect = " << _aspectRatio << " fov = " << fov << std::endl;

	float viewCenter = _info.HScreenSize * 0.25f;
	float eyeProjectionShift = viewCenter - _info.LensSeparationDistance * 0.5f;
	_projectionCenterOffset = 4.0f * eyeProjectionShift / _info.HScreenSize;
#endif

	initFramebuffer();
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

void Application::initFramebuffer()
{
	_fbWidth = _width;
	_fbHeight = _height * 2;


	glGenFramebuffers(2, _framebufferId);
	glGenTextures(2, _renderTexId);

	for (int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId[i]);


		//Создаем текстуру, куда будет осуществляться рендеринг	

		glBindTexture(GL_TEXTURE_2D, _renderTexId[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _fbWidth, _fbHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _renderTexId[i], 0);


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
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Application::draw()
{	
	if (demoNum == 1)
	{
		drawSceneSimple();
	}
	else if (demoNum == 2)
	{
		drawSceneSimpleOculus();
	}
	else if (demoNum == 3)
	{
		drawSceneOculus();
	}

	//TwDraw();

	glfwSwapBuffers(_window);
}

void Application::drawSceneSimple()
{
	glm::mat4 projLeft = _mainCamera.getProjMatrix() *
		glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0, 0.0f,
		_iod * 0.5 / _sd, 0.0f, 1.0, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	glm::mat4 projRight = _mainCamera.getProjMatrix() *
		glm::mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0, 0.0f,
		-_iod * 0.5 / _sd, 0.0f, 1.0, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	glm::mat4 viewLeft = glm::translate(glm::mat4(1.0), glm::vec3(_iod * 0.5, 0, 0)) * _mainCamera.getViewMatrix();
	glm::mat4 viewRight = glm::translate(glm::mat4(1.0), glm::vec3(-_iod * 0.5, 0, 0)) * _mainCamera.getViewMatrix();


	glClearColor(199.0f / 255, 221.0f / 255, 235.0f / 255, 1); //blue color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	
	glViewport(0, 0, _width / 2, _height);
	drawScene(viewLeft, projLeft); //left
	
	glViewport(_width / 2, 0, _width / 2, _height);
	drawScene(viewRight, projRight); //right
}

void Application::drawSceneSimpleOculus()
{
#ifdef OCULUS_FOUND
	glm::mat4 projLeft = glm::translate(glm::mat4(1.0), glm::vec3(_projectionCenterOffset, 0, 0)) * _mainCamera.getProjMatrix();
	glm::mat4 projRight = glm::translate(glm::mat4(1.0), glm::vec3(-_projectionCenterOffset, 0, 0)) * _mainCamera.getProjMatrix();

	float halfIPD = _info.InterpupillaryDistance * 0.5f;

	glm::mat4 viewLeft = glm::translate(glm::mat4(1.0), glm::vec3(halfIPD, 0, 0)) * _mainCamera.getViewMatrix();
	glm::mat4 viewRight = glm::translate(glm::mat4(1.0), glm::vec3(-halfIPD, 0, 0)) * _mainCamera.getViewMatrix();


	glClearColor(199.0f / 255, 221.0f / 255, 235.0f / 255, 1); //blue color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glViewport(0, 0, _width / 2, _height);
	drawScene(viewLeft, projLeft); //left

	
	glViewport(_width / 2, 0, _width / 2, _height);
	drawScene(viewRight, projRight); //right
#endif
}

void Application::drawSceneOculus()
{
#ifdef OCULUS_FOUND
	glm::mat4 projLeft = glm::translate(glm::mat4(1.0), glm::vec3(_projectionCenterOffset, 0, 0)) * _mainCamera.getProjMatrix();
	glm::mat4 projRight = glm::translate(glm::mat4(1.0), glm::vec3(-_projectionCenterOffset, 0, 0)) * _mainCamera.getProjMatrix();

	float halfIPD = _info.InterpupillaryDistance * 0.5f;

	glm::mat4 viewLeft = glm::translate(glm::mat4(1.0), glm::vec3(halfIPD, 0, 0)) * _mainCamera.getViewMatrix();
	glm::mat4 viewRight = glm::translate(glm::mat4(1.0), glm::vec3(-halfIPD, 0, 0)) * _mainCamera.getViewMatrix();



	glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId[0]);

	glClearColor(199.0f / 255, 221.0f / 255, 235.0f / 255, 1); //blue color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glViewport(0, 0, _fbWidth, _fbHeight);

	drawScene(viewLeft, projLeft); //left

	glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId[1]);

	glClearColor(199.0f / 255, 221.0f / 255, 235.0f / 255, 1); //blue color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	glViewport(0, 0, _fbWidth, _fbHeight);

	drawScene(viewRight, projRight); //right


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_DEPTH_BUFFER_BIT);	

	glViewport(0, 0, _width / 2, _height);	
	drawPostprocess(true, _renderTexId[0]);

	glViewport(_width / 2, 0, _width / 2, _height);	
	drawPostprocess(false, _renderTexId[1]);
#endif
}

void Application::drawScene(glm::mat4& viewMat, glm::mat4& projMat)
{
	glUseProgram(_commonMaterial.getProgramId());

	_commonMaterial.setTime((float)glfwGetTime());
	_commonMaterial.setViewMatrix(viewMat);
	_commonMaterial.setProjectionMatrix(projMat);

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


	//====== Плоскость земли ======
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brickTexId);
	glBindSampler(0, _repeatSampler);

	_commonMaterial.setDiffuseTexUnit(0); //текстурный юнит 0	
	_commonMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));	
	_commonMaterial.applyModelSpecificUniforms();

	glBindVertexArray(_ground.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _ground.getNumVertices()); //Рисуем

	glUseProgram(0);
}

void Application::drawPostprocess(bool left, GLuint texId)
{
#ifdef OCULUS_FOUND
	glUseProgram(_oculusDistortionShader.getProgramId());

	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, texId);
	glBindSampler(0, _sampler);

	glm::vec2 lensCenter;
	if (left)
	{
		lensCenter = glm::vec2(1.0f - _info.LensSeparationDistance / _info.HScreenSize, 0.5f);
	}
	else
	{
		lensCenter = glm::vec2(_info.LensSeparationDistance / _info.HScreenSize, 0.5f);
	}

	_oculusDistortionShader.setLensCenter(lensCenter);
	_oculusDistortionShader.setScreenCenter(glm::vec2(0.5f, 0.5f));
	_oculusDistortionShader.setScale(glm::vec2(0.5f / _scaleFactor, 0.5f * _aspectRatio / _scaleFactor));
	_oculusDistortionShader.setScaleIn(glm::vec2(2.0f, 2.0f / _aspectRatio));
	_oculusDistortionShader.setWarpParams(glm::vec4(_info.DistortionK[0], _info.DistortionK[1], _info.DistortionK[2], _info.DistortionK[3]));

	_oculusDistortionShader.setTexUnit(0); //текстурный юнит 0
	_oculusDistortionShader.applyModelSpecificUniforms();

	glBindVertexArray(_screenQuad.getVao()); //Подключаем VertexArray
	glDrawArrays(GL_TRIANGLES, 0, _screenQuad.getNumVertices()); //Рисуем
#endif
}