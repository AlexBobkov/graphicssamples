#include "Application.h"

const int demoNumber = 5; //1 - simple, 2 - mat, 3 - color, 4 - point, 5 - time, 6 - color time, 7 - time coords, 8 - discard, 9 - line, 10 - quad, 11 - fractal

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
	if (demoNumber == 9)	
	{
		makeSceneImplementationLine();
	}
	else if (demoNumber >= 10)	
	{
		makeSceneImplementationQuad();
	}
	else
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

	if (demoNumber == 9)
	{
		drawImplementationLine();
	}
	else if (demoNumber >= 10)
	{
		drawImplementationQuad();
	}
	else
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

	std::string vertFilename = "shaders3/shader.vert";
	std::string fragFilename = "shaders3/shader.frag";

	if (demoNumber == 1)
	{
		vertFilename = "shaders3/simple.vert";
		fragFilename = "shaders3/simple.frag";
	}
	else if (demoNumber == 2)
	{
		vertFilename = "shaders3/simpleMat.vert";
		fragFilename = "shaders3/simple.frag";
	}
	else if (demoNumber == 3)
	{
		vertFilename = "shaders3/shader.vert";
		fragFilename = "shaders3/shader.frag";
	}
	else if (demoNumber == 4)
	{
		vertFilename = "shaders3/shaderPoint.vert";
		fragFilename = "shaders3/shader.frag";
	}
	else if (demoNumber == 5)
	{
		vertFilename = "shaders3/shaderTime.vert";
		fragFilename = "shaders3/shader.frag";
	}
	else if (demoNumber == 6)
	{
		vertFilename = "shaders3/shader.vert";
		fragFilename = "shaders3/shaderTime.frag";
	}
	else if (demoNumber == 7)
	{
		vertFilename = "shaders3/shaderTimeCoord.vert";
		fragFilename = "shaders3/shaderTimeCoord.frag";
	}
	else if (demoNumber == 8)
	{
		vertFilename = "shaders3/shader.vert";
		fragFilename = "shaders3/shaderDiscard.frag";
	}

	GLuint vs = createShader(GL_VERTEX_SHADER, vertFilename);
	GLuint fs = createShader(GL_FRAGMENT_SHADER, fragFilename);

	_shaderProgram = glCreateProgram();
	glAttachShader(_shaderProgram, fs);
	glAttachShader(_shaderProgram, vs);
	glLinkProgram(_shaderProgram);

	//Проверяем ошибки линковки
	int status = -1;
	glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		std::cerr << "Failed to link the program:\n";		
		
		GLint errorLength;
		glGetProgramiv(_shaderProgram, GL_INFO_LOG_LENGTH, &errorLength);

		GLchar* log = new GLchar[errorLength];
		glGetProgramInfoLog(_shaderProgram, errorLength, 0, log);

		std::cerr << log << std::endl;

		delete[] log;
		exit(1);
	}

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


	_timeUniform = glGetUniformLocation(_shaderProgram, "time");
}

void Application::makeSceneImplementationLine()
{
	std::vector<float> vertices;
	for (unsigned int i = 0; i < 36; i++)
	{
		vertices.push_back((float)i);
	}

	unsigned int vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	_vao = 0;
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, NULL);

	std::string vertFilename = "shaders3/shaderLine.vert";
	std::string fragFilename = "shaders3/shader.frag";

	GLuint vs = createShader(GL_VERTEX_SHADER, vertFilename);
	GLuint fs = createShader(GL_FRAGMENT_SHADER, fragFilename);

	_shaderProgram = glCreateProgram();
	glAttachShader(_shaderProgram, fs);
	glAttachShader(_shaderProgram, vs);
	glLinkProgram(_shaderProgram);

	//Проверяем ошибки линковки
	int status = -1;
	glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		std::cerr << "Failed to link the program:\n";		
		
		GLint errorLength;
		glGetProgramiv(_shaderProgram, GL_INFO_LOG_LENGTH, &errorLength);

		GLchar* log = new GLchar[errorLength];
		glGetProgramInfoLog(_shaderProgram, errorLength, 0, log);

		std::cerr << log << std::endl;

		delete[] log;
		exit(1);
	}

	//=========================================================

	_modelMatrixUniform = glGetUniformLocation(_shaderProgram, "modelMatrix");
	_viewMatrixUniform = glGetUniformLocation(_shaderProgram, "viewMatrix");
	_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");

	_modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));		
	_viewMatrix = glm::lookAt(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	_projMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.f);

	_timeUniform = glGetUniformLocation(_shaderProgram, "time");
}

void Application::makeSceneImplementationQuad()
{
	float size = 1.0;

	std::vector<float> vertices;
	addPoint(vertices, -size, size, 0.0);
	addPoint(vertices, size, size, 0.0);
	addPoint(vertices, size, -size, 0.0);

	addPoint(vertices, -size, size, 0.0);
	addPoint(vertices, size, -size, 0.0);
	addPoint(vertices, -size, -size, 0.0);

	unsigned int vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	_vao = 0;
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	std::string vertFilename = "shaders3/shaderQuad.vert";
	std::string fragFilename = "shaders3/shaderQuad.frag";

	if (demoNumber == 11)
	{
		fragFilename = "shaders3/shaderQuadMandelbrot.frag";
	}

	GLuint vs = createShader(GL_VERTEX_SHADER, vertFilename);
	GLuint fs = createShader(GL_FRAGMENT_SHADER, fragFilename);

	_shaderProgram = glCreateProgram();
	glAttachShader(_shaderProgram, fs);
	glAttachShader(_shaderProgram, vs);
	glLinkProgram(_shaderProgram);

	//Проверяем ошибки линковки
	int status = -1;
	glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		std::cerr << "Failed to link the program:\n";		
		
		GLint errorLength;
		glGetProgramiv(_shaderProgram, GL_INFO_LOG_LENGTH, &errorLength);

		GLchar* log = new GLchar[errorLength];
		glGetProgramInfoLog(_shaderProgram, errorLength, 0, log);

		std::cerr << log << std::endl;

		delete[] log;
		exit(1);
	}

	//=========================================================

	_timeUniform = glGetUniformLocation(_shaderProgram, "time");
}

void Application::drawImplementation()
{
	glUseProgram(_shaderProgram);

	glUniform1f(_timeUniform, (float)glfwGetTime()); //передаем время в шейдер	

	glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
	glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));

	glBindVertexArray(_vao);

	_modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_modelMatrix));	

	glDrawArrays(GL_TRIANGLES, 0, 36); //Рисуем куб

	_modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_modelMatrix));	

	glDrawArrays(GL_TRIANGLES, 0, 36); //Рисуем куб
}

void Application::drawImplementationLine()
{
	glUseProgram(_shaderProgram);

	glUniform1f(_timeUniform, (float)glfwGetTime()); //передаем время в шейдер	

	glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
	glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));
	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_modelMatrix));	

	glBindVertexArray(_vao);			
	glDrawArrays(GL_LINE_STRIP, 0, 36);
}

void Application::drawImplementationQuad()
{
	glUseProgram(_shaderProgram);

	glUniform1f(_timeUniform, (float)glfwGetTime()); //передаем время в шейдер	

	glBindVertexArray(_vao);			
	glDrawArrays(GL_TRIANGLES, 0, 6);
}