#include "Application.h"

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

void Application::start()
{
	initContext();
	initGL();
	makeScene();
	run();
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

	glfwSetKeyCallback(_window, keyCallback); //Регистрирует функцию обратного вызова для обработки событий клавиатуры
	glfwSetWindowUserPointer(_window, this); //Регистрируем указатель на данный объект, чтобы потом использовать его в функциях обратного вызова}
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
}

void Application::run()
{
	while (!glfwWindowShouldClose(_window))
	{
		glfwPollEvents();

		update();

		draw();

		glfwSwapBuffers(_window);
	}
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

	//_projMatrix = glm::perspective(glm::radians(_fov), (float)width / height, 0.1f, 100.f);	
}

//==================================================================

GLuint Application::createShader(GLenum shaderType, const std::string& filename)
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

	//Проверяем ошибки компиляции
	int status = -1;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		std::cerr << "Failed to compile the shader:\n";		
		
		GLint errorLength;
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &errorLength);

		GLchar* log = new GLchar[errorLength];
		glGetShaderInfoLog(vs, errorLength, 0, log);

		std::cerr << log << std::endl;

		delete[] log;
		exit(1);
	}

	return vs;
}

GLuint Application::createProgram(const std::string& vertFilename, const std::string& fragFilename)
{
	GLuint vs = createShader(GL_VERTEX_SHADER, vertFilename);
	GLuint fs = createShader(GL_FRAGMENT_SHADER, fragFilename);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, fs);
	glAttachShader(shaderProgram, vs);
	glLinkProgram(shaderProgram);

	//Проверяем ошибки линковки
	int status = -1;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		std::cerr << "Failed to link the program:\n";

		GLint errorLength;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &errorLength);

		GLchar* log = new GLchar[errorLength];
		glGetProgramInfoLog(shaderProgram, errorLength, 0, log);

		std::cerr << log << std::endl;

		delete[] log;
		exit(1);
	}

	return shaderProgram;
}
