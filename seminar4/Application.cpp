#define _USE_MATH_DEFINES
#include <math.h>

#include "Application.h"

bool addColorAttribute = false;
bool addNormalAttribute = true;

int demoNum = 7; //only with addNormalAttribute == true
//1 - diffuse per vertex directinal light
//2 - diffuse per vertex point light
//3 - diffuse per fragment point light
//4 - specular phong
//5 - specular blinn
//6 - materials
//7 - attenuation

//====================================== Вспомогательные функции

float frand(float low, float high)
{
	return low + (high - low) * (rand() % 1000) * 0.001f;
}

void addPoint(std::vector<float>& vec, float x, float y, float z)
{
	vec.push_back(x);
	vec.push_back(y);
	vec.push_back(z);
}

void addColor(std::vector<float>& vec, float r, float g, float b, float a)
{
	vec.push_back(r);
	vec.push_back(g);
	vec.push_back(b);
	vec.push_back(a);
}

void getColorFromLinearPalette(float value, float& r, float& g, float& b)
{
	if (value < 0.25f)
	{
		r = 0.0f;
		g = value * 4.0f;
		b = 1.0f;
	}
	else if (value < 0.5f)
	{
		r = 0.0f;
		g = 1.0f;
		b = (0.5f - value) * 4.0f;
	}
	else if (value < 0.75f)
	{
		r = (value - 0.5f) * 4.0f;
		g = 1.0f;
		b = 0.0f;
	}
	else
	{
		r = 1.0f;
		g = (1.0f - value) * 4.0f;
		b = 0.0f;
	}
}

//======================================

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
			app->fovInc(true);
		}
		else if (key == GLFW_KEY_F)
		{
			app->fovDec(true);
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
			app->fovInc(false);
		}
		else if (key == GLFW_KEY_F)
		{
			app->fovDec(false);
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
	_fovInc(false),
	_fovDec(false),
	_fov(45.0),
	_z(0.0f)
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

	//Включает сглаживание точек
	glEnable(GL_POINT_SMOOTH);

	//Включает изменение размера точек через шейдер
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
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

	_projMatrix = glm::perspective(45.0f, (float)width / height, 0.1f, 100.f);

	drawImplementation();

	glfwSwapBuffers(_window);	
}

void Application::update()
{
	double dt = glfwGetTime() - _oldTime;
	_oldTime = glfwGetTime();

	double speed = 1.0;

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

	glm::vec3 pos = glm::vec3(glm::cos(_phiAng) * glm::cos(_thetaAng), glm::sin(_phiAng) * glm::cos(_thetaAng), glm::sin(_thetaAng)) * 5.0f;

	_viewMatrix = glm::lookAt(pos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}

GLuint Application::createShader(GLenum shaderType, std::string filename)
{
	//Читаем текст шейдера из файла
	std::ifstream vertFile(filename.c_str());
	if (vertFile.fail())
	{
		std::cerr << "Failed to load shader file " << filename << std::endl;
		exit(1);
	}
	std::string vertFileContent((std::istreambuf_iterator<char>(vertFile)), (std::istreambuf_iterator<char>()));
	vertFile.close();

	const char* vertexShaderText = vertFileContent.c_str();

	//Создаем шейдерный объект в OpenGL
	GLuint vs = glCreateShader(shaderType);
	glShaderSource(vs, 1, &vertexShaderText, NULL);
	glCompileShader(vs);

	return vs;
}

void Application::makeSceneImplementation()
{
	makeSphere();
	makeCube();
	makeShaders();
}

void Application::makeSphere()
{
	float radius = 0.5f;
	int N = 100;
	int M = 50;
	_sphereNumTris = 0;

	std::vector<float> vertices;
	std::vector<float> colors;
	std::vector<float> normals;
	for (int i = 0; i < M; i++)
	{
		float theta = M_PI * i / M;
		float theta1 = M_PI * (i + 1) / M;

		for (int j = 0; j < N; j++)
		{
			float phi = 2.0 * M_PI * j / N;
			float phi1 = 2.0 * M_PI * (j + 1) / N;

			//Первый треугольник, образующий квад
			addPoint(vertices, cos(phi) * sin(theta) * radius, sin(phi) * sin(theta) * radius, cos(theta) * radius);
			addPoint(vertices, cos(phi1) * sin(theta) * radius, sin(phi1) * sin(theta) * radius, cos(theta) * radius);
			addPoint(vertices, cos(phi1) * sin(theta1) * radius, sin(phi1) * sin(theta1) * radius, cos(theta1) * radius);

			float r, g, b;
			getColorFromLinearPalette(frand(0.0, 1.0), r, g, b);

			addColor(colors, r, g, b, 1.0);
			addColor(colors, r, g, b, 1.0);
			addColor(colors, r, g, b, 1.0);

			addPoint(normals, cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
			addPoint(normals, cos(phi1) * sin(theta), sin(phi1) * sin(theta), cos(theta));
			addPoint(normals, cos(phi1) * sin(theta1), sin(phi1) * sin(theta1), cos(theta1));

			_sphereNumTris++;

			//Второй треугольник, образующий квад
			addPoint(vertices, cos(phi) * sin(theta) * radius, sin(phi) * sin(theta) * radius, cos(theta) * radius);
			addPoint(vertices, cos(phi1) * sin(theta1) * radius, sin(phi1) * sin(theta1) * radius, cos(theta1) * radius);
			addPoint(vertices, cos(phi) * sin(theta1) * radius, sin(phi) * sin(theta1) * radius, cos(theta1) * radius);			

			getColorFromLinearPalette(frand(0.0, 1.0), r, g, b);

			addColor(colors, r, g, b, 1.0);
			addColor(colors, r, g, b, 1.0);
			addColor(colors, r, g, b, 1.0);

			addPoint(normals, cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
			addPoint(normals, cos(phi1) * sin(theta1), sin(phi1) * sin(theta1), cos(theta1));
			addPoint(normals, cos(phi) * sin(theta1), sin(phi) * sin(theta1), cos(theta1));			

			_sphereNumTris++;
		}
	}

	if (addColorAttribute)
	{
		vertices.insert(vertices.end(), colors.begin(), colors.end());
	}
	else if (addNormalAttribute)
	{
		vertices.insert(vertices.end(), normals.begin(), normals.end());
	}

	unsigned int vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	_sphereVao = 0;
	glGenVertexArrays(1, &_sphereVao);
	glBindVertexArray(_sphereVao);
	glEnableVertexAttribArray(0);	
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	if (addColorAttribute)
	{
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(_sphereNumTris * 3 * 3 * 4));
	}
	else if (addNormalAttribute)
	{
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(_sphereNumTris * 3 * 3 * 4));
	}
	glBindVertexArray(0);

	_sphereModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Application::makeCube()
{
	float size = 0.5f;

	std::vector<float> vertices;
	std::vector<float> colors;
	std::vector<float> normals;

	//front 1
	addPoint(vertices, size, -size, size);
	addPoint(vertices, size, size, size);
	addPoint(vertices, size, size, -size);

	addColor(colors, 1.0, 0.0, 0.0, 1.0);
	addColor(colors, 1.0, 0.0, 0.0, 1.0);
	addColor(colors, 1.0, 0.0, 0.0, 1.0);

	addPoint(normals, 1.0, 0.0, 0.0);
	addPoint(normals, 1.0, 0.0, 0.0);
	addPoint(normals, 1.0, 0.0, 0.0);

	//front 2
	addPoint(vertices, size, -size, size);
	addPoint(vertices, size, size, -size);
	addPoint(vertices, size, -size, -size);

	addColor(colors, 1.0, 0.0, 0.0, 1.0);
	addColor(colors, 1.0, 0.0, 0.0, 1.0);
	addColor(colors, 1.0, 0.0, 0.0, 1.0);

	addPoint(normals, 1.0, 0.0, 0.0);
	addPoint(normals, 1.0, 0.0, 0.0);
	addPoint(normals, 1.0, 0.0, 0.0);

	//left 1
	addPoint(vertices, -size, -size, size);
	addPoint(vertices, size, -size, size);
	addPoint(vertices, size, -size, -size);

	addColor(colors, 1.0, 1.0, 0.0, 1.0);
	addColor(colors, 1.0, 1.0, 0.0, 1.0);
	addColor(colors, 1.0, 1.0, 0.0, 1.0);	

	addPoint(normals, 0.0, -1.0, 0.0);
	addPoint(normals, 0.0, -1.0, 0.0);
	addPoint(normals, 0.0, -1.0, 0.0);	

	//left 2
	addPoint(vertices, -size, -size, size);
	addPoint(vertices, size, -size, -size);
	addPoint(vertices, -size, -size, -size);

	addColor(colors, 1.0, 1.0, 0.0, 1.0);
	addColor(colors, 1.0, 1.0, 0.0, 1.0);
	addColor(colors, 1.0, 1.0, 0.0, 1.0);

	addPoint(normals, 0.0, -1.0, 0.0);
	addPoint(normals, 0.0, -1.0, 0.0);
	addPoint(normals, 0.0, -1.0, 0.0);

	//top 1
	addPoint(vertices, -size, size, size);
	addPoint(vertices, size, size, size);
	addPoint(vertices, size, -size, size);

	addColor(colors, 0.0, 1.0, 0.0, 1.0);
	addColor(colors, 0.0, 1.0, 0.0, 1.0);
	addColor(colors, 0.0, 1.0, 0.0, 1.0);

	addPoint(normals, 0.0, 0.0, 1.0);
	addPoint(normals, 0.0, 0.0, 1.0);
	addPoint(normals, 0.0, 0.0, 1.0);

	//top 2
	addPoint(vertices, -size, size, size);
	addPoint(vertices, -size, -size, size);
	addPoint(vertices, size, -size, size);

	addColor(colors, 0.0, 1.0, 0.0, 1.0);
	addColor(colors, 0.0, 1.0, 0.0, 1.0);
	addColor(colors, 0.0, 1.0, 0.0, 1.0);

	addPoint(normals, 0.0, 0.0, 1.0);
	addPoint(normals, 0.0, 0.0, 1.0);
	addPoint(normals, 0.0, 0.0, 1.0);

	//back 1
	addPoint(vertices, -size, -size, size);
	addPoint(vertices, -size, size, -size);
	addPoint(vertices, -size, size, size);

	addColor(colors, 0.0, 0.0, 1.0, 1.0);
	addColor(colors, 0.0, 0.0, 1.0, 1.0);
	addColor(colors, 0.0, 0.0, 1.0, 1.0);

	addPoint(normals, -1.0, 0.0, 0.0);
	addPoint(normals, -1.0, 0.0, 0.0);
	addPoint(normals, -1.0, 0.0, 0.0);

	//back 2
	addPoint(vertices, -size, -size, size);
	addPoint(vertices, -size, -size, -size);
	addPoint(vertices, -size, size, -size);

	addColor(colors, 0.0, 0.0, 1.0, 1.0);
	addColor(colors, 0.0, 0.0, 1.0, 1.0);
	addColor(colors, 0.0, 0.0, 1.0, 1.0);

	addPoint(normals, -1.0, 0.0, 0.0);
	addPoint(normals, -1.0, 0.0, 0.0);
	addPoint(normals, -1.0, 0.0, 0.0);

	//right 1
	addPoint(vertices, -size, size, size);
	addPoint(vertices, size, size, -size);
	addPoint(vertices, size, size, size);

	addColor(colors, 0.0, 1.0, 1.0, 1.0);
	addColor(colors, 0.0, 1.0, 1.0, 1.0);
	addColor(colors, 0.0, 1.0, 1.0, 1.0);

	addPoint(normals, 0.0, 1.0, 0.0);
	addPoint(normals, 0.0, 1.0, 0.0);
	addPoint(normals, 0.0, 1.0, 0.0);

	//right 2
	addPoint(vertices, -size, size, size);
	addPoint(vertices, -size, size, -size);
	addPoint(vertices, +size, size, -size);

	addColor(colors, 0.0, 1.0, 1.0, 1.0);
	addColor(colors, 0.0, 1.0, 1.0, 1.0);
	addColor(colors, 0.0, 1.0, 1.0, 1.0);

	addPoint(normals, 0.0, 1.0, 0.0);
	addPoint(normals, 0.0, 1.0, 0.0);
	addPoint(normals, 0.0, 1.0, 0.0);

	//bottom 1
	addPoint(vertices, -size, size, -size);
	addPoint(vertices, size, -size, -size);
	addPoint(vertices, size, size, -size);

	addColor(colors, 1.0, 0.0, 1.0, 1.0);
	addColor(colors, 1.0, 0.0, 1.0, 1.0);
	addColor(colors, 1.0, 0.0, 1.0, 1.0);

	addPoint(normals, 0.0, 0.0, -1.0);
	addPoint(normals, 0.0, 0.0, -1.0);
	addPoint(normals, 0.0, 0.0, -1.0);

	//bottom 2
	addPoint(vertices, -size, size, -size);
	addPoint(vertices, size, -size, -size);
	addPoint(vertices, -size, -size, -size);

	addColor(colors, 1.0, 0.0, 1.0, 1.0);
	addColor(colors, 1.0, 0.0, 1.0, 1.0);
	addColor(colors, 1.0, 0.0, 1.0, 1.0);

	addPoint(normals, 0.0, 0.0, -1.0);
	addPoint(normals, 0.0, 0.0, -1.0);
	addPoint(normals, 0.0, 0.0, -1.0);


	if (addColorAttribute)
	{
		vertices.insert(vertices.end(), colors.begin(), colors.end());
	}
	else if (addNormalAttribute)
	{
		vertices.insert(vertices.end(), normals.begin(), normals.end());
	}

	_cubeNumTris = 12;

	unsigned int vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	_cubeVao = 0;
	glGenVertexArrays(1, &_cubeVao);
	glBindVertexArray(_cubeVao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	if (addColorAttribute)
	{
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(_cubeNumTris * 3 * 3 * 4));
	}
	else if (addNormalAttribute)
	{
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(_cubeNumTris * 3 * 3 * 4));
	}
	glBindVertexArray(0);

	_cubeModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
}

void Application::makeShaders()
{
	std::string vertFilename = "shaders4/colored.vert";
	std::string fragFilename = "shaders4/colored.frag";

	if (addColorAttribute)
	{
		
	}
	else if (addNormalAttribute)
	{
		if (demoNum == 1)
		{
			vertFilename = "shaders4/diffusePerVertex.vert";
			fragFilename = "shaders4/diffusePerVertex.frag";
		}
		else if (demoNum == 2)
		{
			vertFilename = "shaders4/diffusePerVertexPoint.vert";
			fragFilename = "shaders4/diffusePerVertexPoint.frag";
		}
		else if (demoNum == 3)
		{
			vertFilename = "shaders4/diffusePerFramentPoint.vert";
			fragFilename = "shaders4/diffusePerFramentPoint.frag";
		}
		else if (demoNum == 4)
		{
			vertFilename = "shaders4/specular.vert";
			fragFilename = "shaders4/specular.frag";
		}
		else if (demoNum == 5)
		{
			vertFilename = "shaders4/specularBlinn.vert";
			fragFilename = "shaders4/specularBlinn.frag";
		}
		else if (demoNum == 6)
		{
			vertFilename = "shaders4/specularBlinnMaterial.vert";
			fragFilename = "shaders4/specularBlinnMaterial.frag";
		}
		else if (demoNum == 7)
		{
			vertFilename = "shaders4/specularBlinnAttenuation.vert";
			fragFilename = "shaders4/specularBlinnAttenuation.frag";
		}
	}

	GLuint vs = createShader(GL_VERTEX_SHADER, vertFilename);
	GLuint fs = createShader(GL_FRAGMENT_SHADER, fragFilename);

	_shaderProgram = glCreateProgram();
	glAttachShader(_shaderProgram, fs);
	glAttachShader(_shaderProgram, vs);
	glLinkProgram(_shaderProgram);

	//=========================================================

	_timeUniform = glGetUniformLocation(_shaderProgram, "time");
	_modelMatrixUniform = glGetUniformLocation(_shaderProgram, "modelMatrix");
	_viewMatrixUniform = glGetUniformLocation(_shaderProgram, "viewMatrix");
	_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");
	_normalToCameraMatrixUniform = glGetUniformLocation(_shaderProgram, "normalToCameraMatrix");
	
	_viewMatrix = glm::lookAt(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	_projMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.f);

	//=========================================================

	_lightDirUniform = glGetUniformLocation(_shaderProgram, "lightDir");
	_lightPosUniform = glGetUniformLocation(_shaderProgram, "lightPos");
	_ambientColorUniform = glGetUniformLocation(_shaderProgram, "ambientColor");
	_diffuseColorUniform = glGetUniformLocation(_shaderProgram, "diffuseColor");
	_specularColorUniform = glGetUniformLocation(_shaderProgram, "specularColor");
	_shininessUniform = glGetUniformLocation(_shaderProgram, "shininessFactor");
	_materialUniform = glGetUniformLocation(_shaderProgram, "material");
	_attenuationUniform = glGetUniformLocation(_shaderProgram, "attenuation");

	_lightDir = glm::vec4(0.0f, 1.0f, 0.8f, 0.0f);
	_lightPos = glm::vec4(0.0f, 1.0f, 0.8f, 1.0f);
	_ambientColor = glm::vec3(0.2, 0.2, 0.2);
	_diffuseColor = glm::vec3(0.8, 0.8, 0.8);
	_specularColor = glm::vec3(0.25, 0.25, 0.25);
		
	_sphereShininess = 100.0f;
	_cubeShininess = 10.0f;

	_sphereMaterial = glm::vec3(1.0, 0.0, 0.0);
	_cubeMaterial = glm::vec3(0.0, 1.0, 0.0);

	_attenuation = 1.0f;
}

void Application::drawImplementation()
{
	glUseProgram(_shaderProgram);

	glUniform1f(_timeUniform, (float)glfwGetTime()); //передаем время в шейдер	
	glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
	glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));

	glUniform4fv(_lightDirUniform, 1, glm::value_ptr(_lightDir));
	glUniform4fv(_lightPosUniform, 1, glm::value_ptr(_lightPos));
	glUniform3fv(_ambientColorUniform, 1, glm::value_ptr(_ambientColor));
	glUniform3fv(_diffuseColorUniform, 1, glm::value_ptr(_diffuseColor));
	glUniform3fv(_specularColorUniform, 1, glm::value_ptr(_specularColor));
	glUniform1f(_attenuationUniform, _attenuation);
	
	//====== Сфера ======
	_normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * _sphereModelMatrix)));
	glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(_normalToCameraMatrix));
	
	glUniform3fv(_materialUniform, 1, glm::value_ptr(_sphereMaterial));
	glUniform1f(_shininessUniform, _sphereShininess);

	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_sphereModelMatrix));	

	glBindVertexArray(_sphereVao);
	glDrawArrays(GL_TRIANGLES, 0, _sphereNumTris * 3); //Рисуем сферу


	//====== Куб ======
	_normalToCameraMatrix = glm::mat3(_viewMatrix * _cubeModelMatrix);
	glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(_normalToCameraMatrix));

	glUniform3fv(_materialUniform, 1, glm::value_ptr(_cubeMaterial));
	glUniform1f(_shininessUniform, _cubeShininess);

	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_cubeModelMatrix));

	glBindVertexArray(_cubeVao);
	glDrawArrays(GL_TRIANGLES, 0, _cubeNumTris * 3); //Рисуем куб
}