#include "Application.h"

const int demoNumber = 1; //

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
}

void Application::makeScene()
{
	if (demoNumber == 1)
	{
		makeSceneImplementation();
	}
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

	if (demoNumber == 1)
	{
		drawImplementation();
	}

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

	//====================================
	//_z += dt;
	//_modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, _z));

	//====================================

	//float fovSpeed = 5.0;
	//
	//if (_fovInc)
	//{
	//	_fov += fovSpeed * dt;
	//}
	//if (_fovDec)
	//{
	//	_fov -= fovSpeed * dt;
	//}

	//std::cout << "FOV " << _fov << std::endl;

	//int width, height;
	//glfwGetFramebufferSize(_window, &width, &height);

	//_projMatrix = glm::perspective(_fov, (float)width / height, 0.1f, 100.f);	
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

void Application::makeSceneImplementation()
{
	float size = 0.5f;

	std::vector<float> vertices;
	//front
	addPoint(vertices, size, -size, size);
	addPoint(vertices, size, size, size);
	addPoint(vertices, size, size, -size);

	addPoint(vertices, size, -size, size);
	addPoint(vertices, size, size, -size);
	addPoint(vertices, size, -size, -size);

	//left
	addPoint(vertices, -size, -size, size);
	addPoint(vertices, size, -size, size);
	addPoint(vertices, size, -size, -size);

	addPoint(vertices, -size, -size, size);
	addPoint(vertices, size, -size, -size);
	addPoint(vertices, -size, -size, -size);

	//top
	addPoint(vertices, -size, size, size);
	addPoint(vertices, size, size, size);
	addPoint(vertices, size, -size, size);

	addPoint(vertices, -size, size, size);
	addPoint(vertices, -size, -size, size);
	addPoint(vertices, size, -size, size);

	//back
	addPoint(vertices, -size, -size, size);
	addPoint(vertices, -size, size, -size);
	addPoint(vertices, -size, size, size);

	addPoint(vertices, -size, -size, size);
	addPoint(vertices, -size, -size, -size);
	addPoint(vertices, -size, size, -size);

	//left
	addPoint(vertices, -size, size, size);
	addPoint(vertices, size, size, -size);
	addPoint(vertices, size, size, size);

	addPoint(vertices, -size, size, size);
	addPoint(vertices, -size, size, -size);
	addPoint(vertices, +size, size, -size);

	//bottom
	addPoint(vertices, -size, size, -size);
	addPoint(vertices, size, -size, -size);
	addPoint(vertices, size, size, -size);

	addPoint(vertices, -size, size, -size);
	addPoint(vertices, size, -size, -size);
	addPoint(vertices, -size, -size, -size);

	addColor(vertices, 1.0, 0.0, 0.0, 1.0);
	addColor(vertices, 1.0, 0.0, 0.0, 1.0);
	addColor(vertices, 1.0, 0.0, 0.0, 1.0);

	addColor(vertices, 1.0, 0.0, 0.0, 1.0);
	addColor(vertices, 1.0, 0.0, 0.0, 1.0);
	addColor(vertices, 1.0, 0.0, 0.0, 1.0);

	addColor(vertices, 1.0, 1.0, 0.0, 1.0);
	addColor(vertices, 1.0, 1.0, 0.0, 1.0);
	addColor(vertices, 1.0, 1.0, 0.0, 1.0);

	addColor(vertices, 1.0, 1.0, 0.0, 1.0);
	addColor(vertices, 1.0, 1.0, 0.0, 1.0);
	addColor(vertices, 1.0, 1.0, 0.0, 1.0);

	addColor(vertices, 0.0, 1.0, 0.0, 1.0);
	addColor(vertices, 0.0, 1.0, 0.0, 1.0);
	addColor(vertices, 0.0, 1.0, 0.0, 1.0);

	addColor(vertices, 0.0, 1.0, 0.0, 1.0);
	addColor(vertices, 0.0, 1.0, 0.0, 1.0);
	addColor(vertices, 0.0, 1.0, 0.0, 1.0);

	addColor(vertices, 0.0, 0.0, 1.0, 1.0);
	addColor(vertices, 0.0, 0.0, 1.0, 1.0);
	addColor(vertices, 0.0, 0.0, 1.0, 1.0);

	addColor(vertices, 0.0, 0.0, 1.0, 1.0);
	addColor(vertices, 0.0, 0.0, 1.0, 1.0);
	addColor(vertices, 0.0, 0.0, 1.0, 1.0);

	addColor(vertices, 0.0, 1.0, 1.0, 1.0);
	addColor(vertices, 0.0, 1.0, 1.0, 1.0);
	addColor(vertices, 0.0, 1.0, 1.0, 1.0);

	addColor(vertices, 0.0, 1.0, 1.0, 1.0);
	addColor(vertices, 0.0, 1.0, 1.0, 1.0);
	addColor(vertices, 0.0, 1.0, 1.0, 1.0);

	addColor(vertices, 1.0, 0.0, 1.0, 1.0);
	addColor(vertices, 1.0, 0.0, 1.0, 1.0);
	addColor(vertices, 1.0, 0.0, 1.0, 1.0);

	addColor(vertices, 1.0, 0.0, 1.0, 1.0);
	addColor(vertices, 1.0, 0.0, 1.0, 1.0);
	addColor(vertices, 1.0, 0.0, 1.0, 1.0);

	int Npoints = vertices.size() / 7;

	unsigned int vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	_vao = 0;
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(Npoints * 4 * 3));

	//=========================================================

	GLuint vs = createShader(GL_VERTEX_SHADER, "shaders2/shader.vert");
	GLuint fs = createShader(GL_FRAGMENT_SHADER, "shaders2/shader.frag");

	_shaderProgram = glCreateProgram();
	glAttachShader(_shaderProgram, fs);
	glAttachShader(_shaderProgram, vs);
	glLinkProgram(_shaderProgram);

	//=========================================================

	_modelMatrixUniform = glGetUniformLocation(_shaderProgram, "modelMatrix");
	_viewMatrixUniform = glGetUniformLocation(_shaderProgram, "viewMatrix");
	_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");

	//float projMatrix[16];
	//float fFrustumScale = 1.0f; float fzNear = 0.5f; float fzFar = 3.0f;
	//memset(projMatrix, 0, sizeof(float) * 16);
	//projMatrix[0] = fFrustumScale;
	//projMatrix[5] = fFrustumScale;
	//projMatrix[10] = (fzFar + fzNear) / (fzNear - fzFar);
	//projMatrix[14] = (2 * fzFar * fzNear) / (fzNear - fzFar);
	//projMatrix[11] = -1.0f;

	//glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, projMatrix);

	_modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));		

	_viewMatrix = glm::lookAt(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	_projMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.f);	
}

void Application::drawImplementation()
{
	glUseProgram(_shaderProgram);

	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_modelMatrix));
	glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
	glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));

	glBindVertexArray(_vao);	
	glDrawArrays(GL_TRIANGLES, 0, 36); //Рисуем 3 грани куба (6 треугольников)
}