#include "Application.h"

const int demoNumber = 2; //1,2,3,4 demos for seminar 1

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
			std::cout << "aaaa\n";
		}
		else if (key == GLFW_KEY_D)
		{
			std::cout << "ddddd\n";
		}
	}
}

//======================================

Application::Application()
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
	else if (demoNumber == 2)
	{
		makeSceneImplementation2();
	}
	else if (demoNumber == 3)
	{
		makeSceneImplementation3();
	}
	else 
	{
		makeSceneImplementation4();
	}
}

void Application::run()
{
	glfwSetKeyCallback(_window, keyCallback);

	while (!glfwWindowShouldClose(_window))
	{
		glfwPollEvents();

		draw();
	}
}

void Application::draw()
{
	int width, height;
	glfwGetFramebufferSize(_window, &width, &height);
	glViewport(0, 0, width, height);		
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(_shaderProgram);

	if (demoNumber == 1 || demoNumber == 2)
	{
		drawImplementation();
	}
	else if (demoNumber == 3)
	{
		drawImplementation3();
	}
	else
	{
		drawImplementation4();
	}

	glfwSwapBuffers(_window);	
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
	float points[] =
	{
		0.0f,  0.5f,  0.0f,
		0.5f, -0.5f,  0.0f,
		-0.5f, -0.5f,  0.0f
	};

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof (float), points, GL_STATIC_DRAW);

	_vao = 0;
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//=========================================================

	GLuint vs = createShader(GL_VERTEX_SHADER, "shaders1/simple.vert");
	GLuint fs = createShader(GL_FRAGMENT_SHADER, "shaders1/simple.frag");

	_shaderProgram = glCreateProgram();
	glAttachShader(_shaderProgram, fs);
	glAttachShader(_shaderProgram, vs);
	glLinkProgram(_shaderProgram);

	//=========================================================

	_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");

	float fFrustumScale = 1.0f; float fzNear = 0.5f; float fzFar = 3.0f;

	memset(_projMatrix, 0, sizeof(float) * 16);

	_projMatrix[0] = fFrustumScale;
	_projMatrix[5] = fFrustumScale;
	_projMatrix[10] = (fzFar + fzNear) / (fzNear - fzFar);
	_projMatrix[14] = (2 * fzFar * fzNear) / (fzNear - fzFar);
	_projMatrix[11] = -1.0f;

	glUseProgram(_shaderProgram);
	glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, _projMatrix);
}

void Application::makeSceneImplementation2()
{
	float points[] =
	{
		0.0f,  0.5f,  0.0f,
		0.5f, -0.5f,  0.0f,
		-0.5f, -0.5f,  0.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
	};

	unsigned int vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 21 * sizeof(float), points, GL_STATIC_DRAW);

	_vao = 0;
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)36);

	//=========================================================

	GLuint vs = createShader(GL_VERTEX_SHADER, "shaders1/simple_color.vert");
	GLuint fs = createShader(GL_FRAGMENT_SHADER, "shaders1/simple_color.frag");

	_shaderProgram = glCreateProgram();
	glAttachShader(_shaderProgram, fs);
	glAttachShader(_shaderProgram, vs);
	glLinkProgram(_shaderProgram);

	//=========================================================

	_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");

	float fFrustumScale = 1.0f; float fzNear = 0.5f; float fzFar = 3.0f;

	memset(_projMatrix, 0, sizeof(float) * 16);

	_projMatrix[0] = fFrustumScale;
	_projMatrix[5] = fFrustumScale;
	_projMatrix[10] = (fzFar + fzNear) / (fzNear - fzFar);
	_projMatrix[14] = (2 * fzFar * fzNear) / (fzNear - fzFar);
	_projMatrix[11] = -1.0f;

	glUseProgram(_shaderProgram);
	glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, _projMatrix);
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

void Application::makeSceneImplementation3()
{
	std::vector<float> vertices;
	//front
	addPoint(vertices, -0.3f, 0.3f, 0.0f);
	addPoint(vertices, 0.3f, 0.3f, 0.0f);
	addPoint(vertices, 0.3f, -0.3f, 0.0f);

	addPoint(vertices, -0.3f, 0.3f, 0.0f);
	addPoint(vertices, 0.3f, -0.3f, 0.0f);
	addPoint(vertices, -0.3f, -0.3f, 0.0f);

	//left
	addPoint(vertices, -0.3f, 0.3f, 0.0f);
	addPoint(vertices, -0.3f, -0.3f, -1.0f);
	addPoint(vertices, -0.3f, 0.3f, -1.0f);

	addPoint(vertices, -0.3f, 0.3f, 0.0f);
	addPoint(vertices, -0.3f, -0.3f, 0.0f);
	addPoint(vertices, -0.3f, -0.3f, -1.0f);

	//buttom
	addPoint(vertices, -0.3f, -0.3f, 0.0f);
	addPoint(vertices, 0.3f, -0.3f, -1.0f);
	addPoint(vertices, 0.3f, -0.3f, 0.0f);

	addPoint(vertices, -0.3f, -0.3f, 0.0f);
	addPoint(vertices, -0.3f, -0.3f, -1.0f);
	addPoint(vertices, 0.3f, -0.3f, -1.0f);

	addColor(vertices, 1.0f, 0.0f, 0.0f, 1.0f);
	addColor(vertices, 1.0f, 0.0f, 0.0f, 1.0f);
	addColor(vertices, 1.0f, 0.0f, 0.0f, 1.0f);

	addColor(vertices, 1.0f, 0.0f, 0.0f, 1.0f);
	addColor(vertices, 1.0f, 0.0f, 0.0f, 1.0f);
	addColor(vertices, 1.0f, 0.0f, 0.0f, 1.0f);

	addColor(vertices, 1.0f, 1.0f, 0.0f, 1.0f);
	addColor(vertices, 1.0f, 1.0f, 0.0f, 1.0f);
	addColor(vertices, 1.0f, 1.0f, 0.0f, 1.0f);

	addColor(vertices, 1.0f, 1.0f, 0.0f, 1.0f);
	addColor(vertices, 1.0f, 1.0f, 0.0f, 1.0f);
	addColor(vertices, 1.0f, 1.0f, 0.0f, 1.0f);

	addColor(vertices, 0.0f, 1.0f, 0.0f, 1.0f);
	addColor(vertices, 0.0f, 1.0f, 0.0f, 1.0f);
	addColor(vertices, 0.0f, 1.0f, 0.0f, 1.0f);

	addColor(vertices, 0.0f, 1.0f, 0.0f, 1.0f);
	addColor(vertices, 0.0f, 1.0f, 0.0f, 1.0f);
	addColor(vertices, 0.0f, 1.0f, 0.0f, 1.0f);

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

	GLuint vs = createShader(GL_VERTEX_SHADER, "simple_color.vert");
	GLuint fs = createShader(GL_FRAGMENT_SHADER, "simple_color.frag");

	_shaderProgram = glCreateProgram();
	glAttachShader(_shaderProgram, fs);
	glAttachShader(_shaderProgram, vs);
	glLinkProgram(_shaderProgram);

	//=========================================================

	_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");

	float fFrustumScale = 1.0f; float fzNear = 0.5f; float fzFar = 3.0f;

	memset(_projMatrix, 0, sizeof(float) * 16);

	_projMatrix[0] = fFrustumScale;
	_projMatrix[5] = fFrustumScale;
	_projMatrix[10] = (fzFar + fzNear) / (fzNear - fzFar);
	_projMatrix[14] = (2 * fzFar * fzNear) / (fzNear - fzFar);
	_projMatrix[11] = -1.0f;

	glUseProgram(_shaderProgram);
	glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, _projMatrix);
}

void Application::makeSceneImplementation4()
{
	std::vector<float> vertices;	
	addPoint(vertices, -0.3f, 0.3f, 0.0f);
	addPoint(vertices, 0.3f, 0.3f, 0.0f);
	addPoint(vertices, 0.3f, -0.3f, 0.0f);
	addPoint(vertices, -0.3f, -0.3f, 0.0f);
	addPoint(vertices, -0.3f, 0.3f, -1.0f);
	addPoint(vertices, -0.3f, -0.3f, -1.0f);
	addPoint(vertices, 0.3f, -0.3f, -1.0f);

	addColor(vertices, 1.0f, 0.0f, 0.0f, 1.0f);
	addColor(vertices, 1.0f, 0.0f, 0.0f, 1.0f);
	addColor(vertices, 1.0f, 0.0f, 0.0f, 1.0f);
	addColor(vertices, 0.0f, 0.0f, 1.0f, 1.0f);	
	addColor(vertices, 1.0f, 1.0f, 0.0f, 1.0f);
	addColor(vertices, 1.0f, 1.0f, 0.0f, 1.0f);
	addColor(vertices, 0.0f, 1.0f, 0.0f, 1.0f);
	
	int Npoints = vertices.size() / 7;

	std::vector<unsigned short> indices;
	indices.push_back(0); indices.push_back(1); indices.push_back(2);
	indices.push_back(0); indices.push_back(2); indices.push_back(3);	
	indices.push_back(4); indices.push_back(0); indices.push_back(3);
	indices.push_back(4); indices.push_back(3); indices.push_back(5);
	indices.push_back(5); indices.push_back(6); indices.push_back(2);
	indices.push_back(5); indices.push_back(2); indices.push_back(3);

	unsigned int vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	unsigned int ibo = 0;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), indices.data(), GL_STATIC_DRAW);

	_vao = 0;
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(Npoints * 4 * 3));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	
	//=========================================================

	GLuint vs = createShader(GL_VERTEX_SHADER, "simple_color.vert");
	GLuint fs = createShader(GL_FRAGMENT_SHADER, "simple_color.frag");

	_shaderProgram = glCreateProgram();
	glAttachShader(_shaderProgram, fs);
	glAttachShader(_shaderProgram, vs);
	glLinkProgram(_shaderProgram);

	//=========================================================

	_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");

	float fFrustumScale = 1.0f; float fzNear = 0.5f; float fzFar = 3.0f;

	memset(_projMatrix, 0, sizeof(float) * 16);

	_projMatrix[0] = fFrustumScale;
	_projMatrix[5] = fFrustumScale;
	_projMatrix[10] = (fzFar + fzNear) / (fzNear - fzFar);
	_projMatrix[14] = (2 * fzFar * fzNear) / (fzNear - fzFar);
	_projMatrix[11] = -1.0f;

	glUseProgram(_shaderProgram);
	glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, _projMatrix);
}

void Application::drawImplementation()
{
	glBindVertexArray(_vao);	
	glDrawArrays(GL_TRIANGLES, 0, 3); //Рисуем треугольник
}

void Application::drawImplementation3()
{
	glBindVertexArray(_vao);	
	glDrawArrays(GL_TRIANGLES, 0, 18); //Рисуем 3 грани куба (6 треугольников)
}

void Application::drawImplementation4()
{
	glBindVertexArray(_vao);	
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0); //Рисуем с помощью индексов
}