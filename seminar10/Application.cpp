#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "Application.h"
#include "Texture.h"

int demoNum = 7;
//1 - for cycle for spheres
//2 - static instancing
//3 - hardware instancing
//4 - hardware instancing with uniform
//5 - hardware instancing with texture
//6 - hardware instancing with divisor
//7 - particle system on CPU

int K = 500; //number of instances

int numParticles = 1000;
double emitterSize = 1.0;
int lifeTime = 3.0;

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

	if (demoNum == 3)
	{
		_commonMaterial.setVertFilename("shaders10/common_inst1.vert");
	}
	else if (demoNum == 4)
	{
		_commonMaterial.setVertFilename("shaders10/common_inst2.vert");
	}
	else if (demoNum == 5)
	{
		_commonMaterial.setVertFilename("shaders10/common_inst3.vert");
	}
	else if (demoNum == 6)
	{
		_commonMaterial.setVertFilename("shaders10/common_inst4.vert");
	}

	_commonMaterial.initialize();

	_particleMaterial.initialize();

	//Загружаем текстуры
	_worldTexId = Texture::loadTexture("images/earth_global.jpg");
	_brickTexId = Texture::loadTexture("images/brick.jpg");
	_grassTexId = Texture::loadTexture("images/grass.jpg");
	_rotateTexId = Texture::loadTexture("images/rotate.png");
	_particleTexId = Texture::loadTexture("images/particle.png", true);
	_colorTexId = Texture::makeCustomTexture();	

	//инициализируем положения центров сфер
	float size = 100.0f;
	for (int i = 0; i < K; i++)
	{
		_positions.push_back(glm::vec3(frand() * size - 0.5 * size, frand() * size - 0.5 * size, 0.0));
	}

	if (demoNum == 4)
	{
		_commonMaterial.setPositions(_positions);
	}

	if (demoNum == 5)
	{
		_texBufferId = Texture::makeTextureBuffer(_positions);
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

	if (demoNum == 2)
	{
		_sphereArray = Mesh::makeStaticSphereArray(0.8f, K, _positions);
	}

	if (demoNum == 6)
	{
		_sphere.addInstancedData(3, _positions);
	}

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

	if (demoNum == 7)
	{
		for (unsigned int i = 0; i < numParticles; i++)
		{
			Particle p;
			p.position = glm::vec3((frand() - 0.5) * emitterSize, (frand() - 0.5) * emitterSize, frand() * 5.0);
			p.velocity = glm::vec3(frand() * 0.01, frand() * 0.01, 0.0);
			p.startTime = frand() * lifeTime;
			_particles.push_back(p);
		}

		for (unsigned int i = 0; i < numParticles; i++)	
		{
			addVec3(_particlePositions, _particles[i].position.x, _particles[i].position.y, _particles[i].position.z);

			_particleTimes.push_back(_particles[i].startTime);
		}	

		_particlePosVbo = 0;
		glGenBuffers(1, &_particlePosVbo);
		glBindBuffer(GL_ARRAY_BUFFER, _particlePosVbo);
		glBufferData(GL_ARRAY_BUFFER, _particlePositions.size() * sizeof(float), _particlePositions.data(), GL_STREAM_DRAW);

		_particleTimeVbo = 0;
		glGenBuffers(1, &_particleTimeVbo);
		glBindBuffer(GL_ARRAY_BUFFER, _particleTimeVbo);
		glBufferData(GL_ARRAY_BUFFER, _particleTimes.size() * sizeof(float), _particleTimes.data(), GL_STREAM_DRAW);

		_particleVao = 0;
		glGenVertexArrays(1, &_particleVao);
		glBindVertexArray(_particleVao);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, _particlePosVbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, _particleTimeVbo);
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindVertexArray(0);
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

	float dt = glfwGetTime() - _oldTime;
	_oldTime = glfwGetTime();
	_fps = 1 / dt;

	if (demoNum == 7)
	{
		dt = glm::min(dt, 0.03f);

		for (unsigned int i = 0; i < _particles.size(); i++)
		{
			_particles[i].velocity += glm::vec3(0.0, 0.0, -9.8) * dt;
			_particles[i].position += _particles[i].velocity * dt;

			if (_oldTime - _particles[i].startTime > lifeTime)
			{
				_particles[i].startTime += lifeTime;

				_particles[i].position = glm::vec3((frand() - 0.5) * emitterSize, (frand() - 0.5) * emitterSize, -2.0);
				_particles[i].velocity = glm::vec3(_particles[i].position.x * 0.7, 5.0 + _particles[i].position.y * 0.7, 10.0);
			}

			_particlePositions[i * 3 + 0] = _particles[i].position.x;
			_particlePositions[i * 3 + 1] = _particles[i].position.y;
			_particlePositions[i * 3 + 2] = _particles[i].position.z;

			_particleTimes[i] = _particles[i].startTime;
		}

		glBindBuffer(GL_ARRAY_BUFFER, _particlePosVbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, _particlePositions.size() * sizeof(float), _particlePositions.data());

		glBindBuffer(GL_ARRAY_BUFFER, _particleTimeVbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, _particleTimes.size() * sizeof(float), _particleTimes.data());
	}
}

void Application::draw()
{
	if (demoNum <= 6)
	{
		drawScene(_mainCamera);
	}
	else
	{
		drawParticles(_mainCamera);
	}

	TwDraw();

	glfwSwapBuffers(_window);
}

void Application::drawScene(Camera& camera)
{
	glViewport(0, 0, _width, _height);
	glClearColor(199.0f / 255, 221.0f / 255, 235.0f / 255, 1); //blue color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glUseProgram(_commonMaterial.getProgramId()); //Подключаем шейдер, который рендерит в 3 текстуры: текстуру с нормалями, с глубинами, с диффузным цветом

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
	
	if (demoNum == 1)
	{
		for (unsigned int i = 0; i < _positions.size(); i++)
		{
			_commonMaterial.setModelMatrix(glm::translate(glm::mat4(1.0f), _positions[i])); //считаем матрицу модели, используя координаты центра сферы
			_commonMaterial.applyModelSpecificUniforms();

			glBindVertexArray(_sphere.getVao()); //Подключаем VertexArray
			glDrawArrays(GL_TRIANGLES, 0, _sphere.getNumVertices()); //Рисуем
		}
	}
	else if (demoNum == 2)
	{
		_commonMaterial.setModelMatrix(glm::mat4(1.0f)); //считаем матрицу модели, используя координаты центра сферы
		_commonMaterial.applyModelSpecificUniforms();

		glBindVertexArray(_sphereArray.getVao()); //Подключаем VertexArray
		glDrawArrays(GL_TRIANGLES, 0, _sphereArray.getNumVertices()); //Рисуем
	}
	else if (demoNum == 3 || demoNum == 4 || demoNum == 6)
	{		
		_commonMaterial.setModelMatrix(glm::mat4(1.0f)); //считаем матрицу модели, используя координаты центра сферы
		_commonMaterial.applyModelSpecificUniforms();

		glBindVertexArray(_sphere.getVao()); //Подключаем VertexArray
		glDrawArraysInstanced(GL_TRIANGLES, 0, _sphere.getNumVertices(), K); //Рисуем
	}
	else if (demoNum == 5)
	{
		glActiveTexture(GL_TEXTURE0 + 1);  //текстурный юнит 1
		glBindTexture(GL_TEXTURE_BUFFER, _texBufferId);

		_commonMaterial.setTexBufUnit(1);
		_commonMaterial.setModelMatrix(glm::mat4(1.0f)); //считаем матрицу модели, используя координаты центра сферы
		_commonMaterial.applyModelSpecificUniforms();

		glBindVertexArray(_sphere.getVao()); //Подключаем VertexArray
		glDrawArraysInstanced(GL_TRIANGLES, 0, _sphere.getNumVertices(), K); //Рисуем
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

void Application::drawParticles(Camera& camera)
{
	glViewport(0, 0, _width, _height);
	glClearColor(0, 0, 0, 1); //black color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glUseProgram(_particleMaterial.getProgramId()); //Подключаем шейдер, который рендерит в 3 текстуры: текстуру с нормалями, с глубинами, с диффузным цветом

	_particleMaterial.setTime((float)glfwGetTime());

	_particleMaterial.setModelMatrix(glm::mat4(1.0f));	
	_particleMaterial.setViewMatrix(camera.getViewMatrix());
	_particleMaterial.setProjectionMatrix(camera.getProjMatrix());

	_particleMaterial.setDiffuseTexUnit(0); //текстурный юнит 0	

	_particleMaterial.applyCommonUniforms();
	_particleMaterial.applyModelSpecificUniforms();

	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _particleTexId);
	glBindSampler(0, _sampler);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SPRITE);
	
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	

	glBindVertexArray(_particleVao); //Подключаем VertexArray
	glDrawArrays(GL_POINTS, 0, _particles.size()); //Рисуем		

	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);

	glUseProgram(0);
}