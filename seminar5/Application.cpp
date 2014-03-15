#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "Application.h"

bool useLighting = true;
bool useSpecularTex = false;
bool showChess = false;

//Функция обратного вызова для обработки нажатий на клавиатуре
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Application* app = (Application*)glfwGetWindowUserPointer(window);

	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if (key == GLFW_KEY_A)
		{
			app->rotateLeft(true);
		}
		else if (key == GLFW_KEY_D)
		{
			app->rotateRight(true);
		}
		else if (key == GLFW_KEY_W)
		{
			app->rotateUp(true);
		}
		else if (key == GLFW_KEY_S)
		{
			app->rotateDown(true);
		}
		else if (key == GLFW_KEY_R)
		{
			app->zoomUp(true);
		}
		else if (key == GLFW_KEY_F)
		{
			app->zoomDown(true);
		}
		else if (key == GLFW_KEY_SPACE)
		{
			app->homePos();
		}
	}
	else if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_A)
		{
			app->rotateLeft(false);
		}
		else if (key == GLFW_KEY_D)
		{
			app->rotateRight(false);
		}
		else if (key == GLFW_KEY_W)
		{
			app->rotateUp(false);
		}
		else if (key == GLFW_KEY_S)
		{
			app->rotateDown(false);
		}
		else if (key == GLFW_KEY_R)
		{
			app->zoomUp(false);
		}
		else if (key == GLFW_KEY_F)
		{
			app->zoomDown(false);
		}
	}
}

//======================================

Application::Application():
_oldTime(0.0f),
	_rotateLeft(false),
	_rotateRight(false),
	_phiAng(0.0f),
	_rotateUp(false),
	_rotateDown(false),
	_thetaAng(0.0f),
	_zoomUp(false),
	_zoomDown(false),
	_distance(5.0f)
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

	_projMatrix = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.f);

	drawImplementation();

	glfwSwapBuffers(_window);	
}

void Application::update()
{
	float dt = (float)glfwGetTime() - _oldTime;
	_oldTime = (float)glfwGetTime();

	float speed = 1.0f;

	if (_rotateLeft)
	{
		_phiAng -= speed * dt;
	}
	if (_rotateRight)
	{
		_phiAng += speed * dt;
	}
	if (_rotateUp)
	{
		_thetaAng += speed * dt;
	}
	if (_rotateDown)
	{
		_thetaAng -= speed * dt;
	}
	if (_zoomUp)
	{
		_distance += _distance * dt;
	}
	if (_zoomDown)
	{
		_distance -= _distance * dt;
	}

	_distance = glm::clamp(_distance, 0.5f, 50.0f);

	glm::vec3 pos = glm::vec3(glm::cos(_phiAng) * glm::cos(_thetaAng), glm::sin(_phiAng) * glm::cos(_thetaAng), glm::sin(_thetaAng)) * _distance;

	_viewMatrix = glm::lookAt(pos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Application::homePos()
{
	_phiAng = 0.0f;
	_thetaAng = (float)M_PI * 0.05f;
	_distance = 20.0f;

	glm::vec3 pos = glm::vec3(glm::cos(_phiAng) * glm::cos(_thetaAng), glm::sin(_phiAng) * glm::cos(_thetaAng), glm::sin(_thetaAng)) * _distance;

	_viewMatrix = glm::lookAt(pos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Application::makeSceneImplementation()
{
	makeSphere();
	makeCube();
	makePlane();
	makeChessPlane();
	makeShaders();
	initData();
}

void Application::makeShaders()
{
	std::string vertFilename = "shaders5/texture_without_lighting.vert";
	std::string fragFilename = "shaders5/texture_without_lighting.frag";

	if (useLighting && !showChess)
	{
		vertFilename = "shaders5/shader.vert";
		fragFilename = "shaders5/shader.frag";

		if (useSpecularTex)		
		{
			vertFilename = "shaders5/shader_specular_tex.vert";
			fragFilename = "shaders5/shader_specular_tex.frag";
		}
	}

	_shaderProgram = makeShaderProgram(vertFilename, fragFilename);

	//=========================================================
	//Инициализация uniform-переменных для преобразования координат

	_timeUniform = glGetUniformLocation(_shaderProgram, "time");
	_modelMatrixUniform = glGetUniformLocation(_shaderProgram, "modelMatrix");
	_viewMatrixUniform = glGetUniformLocation(_shaderProgram, "viewMatrix");
	_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");
	_normalToCameraMatrixUniform = glGetUniformLocation(_shaderProgram, "normalToCameraMatrix");	

	//=========================================================
	//Инициализация uniform-переменных для освещения

	_lightDirUniform = glGetUniformLocation(_shaderProgram, "lightDir");
	_lightPosUniform = glGetUniformLocation(_shaderProgram, "lightPos");
	_ambientColorUniform = glGetUniformLocation(_shaderProgram, "ambientColor");
	_diffuseColorUniform = glGetUniformLocation(_shaderProgram, "diffuseColor");
	_specularColorUniform = glGetUniformLocation(_shaderProgram, "specularColor");
	_shininessUniform = glGetUniformLocation(_shaderProgram, "shininessFactor");
	_materialUniform = glGetUniformLocation(_shaderProgram, "material");
	_attenuationUniform = glGetUniformLocation(_shaderProgram, "attenuation");

	//=========================================================
	//Инициализация uniform-переменных для текстурирования

	_diffuseTexUniform = glGetUniformLocation(_shaderProgram, "diffuseTex");
	_specularTexUniform = glGetUniformLocation(_shaderProgram, "specularTex");
}

void Application::initData()
{
	//Инициализация матриц
	_viewMatrix = glm::lookAt(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	_projMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);

	//Инициализация значений переменных освщения
	_lightDir = glm::vec4(0.0f, 1.0f, 0.8f, 0.0f);
	_lightPos = glm::vec4(2.0f, 2.0f, 0.5f, 1.0f);
	_ambientColor = glm::vec3(0.2, 0.2, 0.2);
	_diffuseColor = glm::vec3(0.8, 0.8, 0.8);
	_specularColor = glm::vec3(0.5, 0.5, 0.5);
	_attenuation = 1.0f;

	//Инициализация материалов
	_shininess1 = 100.0f;
	_material1 = glm::vec3(1.0, 0.0, 0.0);

	_shininess2 = 100.0f;	
	_material2 = glm::vec3(0.0, 1.0, 0.0);	

	//Инициализация текстур
	_worldTexId = loadTexture("images/earth_global.jpg");
	_brickTexId = loadTexture("images/brick.jpg");
	_grassTexId = loadTexture("images/grass.jpg");
	_specularTexId = loadTexture("images/specular.dds");
	_chessTexId = loadTextureWithMipmaps("images/chess.dds");
	_myTexId = makeCustomTexture();

	//====
	GLfloat maxAniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);

	std::cout << "Max anistropy " << maxAniso << std::endl;
	//====
			
	//Инициализация сэмплера, объекта, который хранит параметры чтения из текстуры
	glGenSamplers(1, &_sampler);
	glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	int variant = 3;

	glGenSamplers(1, &_repeatSampler);
	if (variant == 0)
	{
		glSamplerParameteri(_repeatSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glSamplerParameteri(_repeatSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else if (variant == 1)
	{
		glSamplerParameteri(_repeatSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_repeatSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	}
	else if (variant == 2)
	{
		glSamplerParameteri(_repeatSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_repeatSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	}
	else if (variant == 3)
	{
		glSamplerParameteri(_repeatSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_repeatSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}

	glSamplerParameterf(_repeatSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);

	glSamplerParameteri(_repeatSampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(_repeatSampler, GL_TEXTURE_WRAP_T, GL_REPEAT);	
}

void Application::drawImplementation()
{
	glUseProgram(_shaderProgram);

	//=============================================================================
	//Копирование на видеокарту значений uniform-пемеренных, общих для всех объектов

	glUniform1f(_timeUniform, (float)glfwGetTime()); //передаем время в шейдер	
	glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
	glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));

	glUniform4fv(_lightDirUniform, 1, glm::value_ptr(_lightDir));
	glUniform4fv(_lightPosUniform, 1, glm::value_ptr(_lightPos));
	glUniform3fv(_ambientColorUniform, 1, glm::value_ptr(_ambientColor));
	glUniform3fv(_diffuseColorUniform, 1, glm::value_ptr(_diffuseColor));
	glUniform3fv(_specularColorUniform, 1, glm::value_ptr(_specularColor));
	glUniform1f(_attenuationUniform, _attenuation);

	glUniform1i(_diffuseTexUniform, 0); //текстурный юнит 0
	glUniform1i(_specularTexUniform, 1); //текстурный юнит 1

		
	//=============================================================================
	//Активизация текстур
	glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
	glBindTexture(GL_TEXTURE_2D, _brickTexId);
	glBindSampler(0, _sampler);

	if (useSpecularTex)
	{
		glActiveTexture(GL_TEXTURE0 + 1);  //текстурный юнит 1
		glBindTexture(GL_TEXTURE_2D, _specularTexId);
		glBindSampler(1, _sampler);
	}	


	//====== Сфера ======
	//Копирование на видеокарту значений uniform-пемеренных для сферы
	_normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * _sphereModelMatrix)));
	glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(_normalToCameraMatrix));

	glUniform3fv(_materialUniform, 1, glm::value_ptr(_material1));
	glUniform1f(_shininessUniform, _shininess1);

	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_sphereModelMatrix));	

	glBindVertexArray(_sphereVao); //Подключаем VertexArray для сферы
	glDrawArrays(GL_TRIANGLES, 0, _sphereNumTris * 3); //Рисуем сферу

	//====== Плоскость ======
	//Копирование на видеокарту значений uniform-пемеренных для плоскости
	_normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * _planeModelMatrix)));
	glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(_normalToCameraMatrix));

	glUniform3fv(_materialUniform, 1, glm::value_ptr(_material2));
	glUniform1f(_shininessUniform, _shininess2);

	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_planeModelMatrix));

	glBindVertexArray(_planeVao); //Подключаем VertexArray для плоскости
	glDrawArrays(GL_TRIANGLES, 0, 6); //Рисуем плоскость

	if (showChess)
	{
		glActiveTexture(GL_TEXTURE0 + 0);  //текстурный юнит 0
		glBindTexture(GL_TEXTURE_2D, _chessTexId);
		glBindSampler(0, _repeatSampler);

		//====== Шахматы ======
		//Копирование на видеокарту значений uniform-пемеренных для плоскости
		_normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix)));
		glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(_normalToCameraMatrix));

		glUniform3fv(_materialUniform, 1, glm::value_ptr(_material2));
		glUniform1f(_shininessUniform, _shininess2);

		glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));

		glBindVertexArray(_chessVao); //Подключаем VertexArray для плоскости
		glDrawArrays(GL_TRIANGLES, 0, 6); //Рисуем плоскость
	}

#if 0
	//====== Куб ======
	//Копирование на видеокарту значений uniform-пемеренных для куба
	_normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * _cubeModelMatrix)));
	glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(_normalToCameraMatrix));

	glUniform3fv(_materialUniform, 1, glm::value_ptr(_material2));
	glUniform1f(_shininessUniform, _shininess2);

	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_cubeModelMatrix));

	glBindVertexArray(_cubeVao); //Подключаем VertexArray для куба
	glDrawArrays(GL_TRIANGLES, 0, _cubeNumTris * 3); //Рисуем куб
#endif
}