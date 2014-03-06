#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "Application.h"

int demoNum = 1;

//====================================== Вспомогательные функции

float frand(float low, float high)
{
	return low + (high - low) * (rand() % 1000) * 0.001f;
}

void addVec2(std::vector<float>& vec, float s, float t)
{
	vec.push_back(s);
	vec.push_back(t);
}

void addVec3(std::vector<float>& vec, float x, float y, float z)
{
	vec.push_back(x);
	vec.push_back(y);
	vec.push_back(z);
}

void addVec4(std::vector<float>& vec, float r, float g, float b, float a)
{
	vec.push_back(r);
	vec.push_back(g);
	vec.push_back(b);
	vec.push_back(a);
}

//вычисление цвета по линейной палитре
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
	_thetaAng(0.0f)	
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

void Application::makeSceneImplementation()
{
	makeSphere();
	makeCube();
	makeShaders();

	//std::string textureFilename = "images/brick.jpg";
	std::string textureFilename = "images/earth_global.jpg";
    
    try
    {
		std::shared_ptr<glimg::ImageSet> pImageSet(glimg::loaders::stb::LoadFromFile(textureFilename));

        glimg::SingleImage pImage = pImageSet->GetImage(0, 0, 0);
        
        glimg::Dimensions dims = pImage.GetDimensions();
        
        glGenTextures(1, &_texId);
        glBindTexture(GL_TEXTURE_2D, _texId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, dims.width, dims.height, 0, GL_RGB, GL_UNSIGNED_BYTE, pImage.GetImageData());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
	}
	catch(glimg::loaders::stb::StbLoaderException &e)
	{
		std::cerr << "Failed to load texture " << textureFilename << std::endl;;
		exit(1);
	}
}

void Application::makeSphere()
{
	float radius = 0.5f;
	int N = 100;
	int M = 50;
	_sphereNumTris = 0;

	std::vector<float> vertices;	
	std::vector<float> normals;
	std::vector<float> texcoords;
	for (int i = 0; i < M; i++)
	{
		float theta = M_PI * i / M;
		float theta1 = M_PI * (i + 1) / M;

		for (int j = 0; j < N; j++)
		{
			float phi = 2.0 * M_PI * j / N;
			float phi1 = 2.0 * M_PI * (j + 1) / N;

			//Первый треугольник, образующий квад
			addVec3(vertices, cos(phi) * sin(theta) * radius, sin(phi) * sin(theta) * radius, cos(theta) * radius);
			addVec3(vertices, cos(phi1) * sin(theta) * radius, sin(phi1) * sin(theta) * radius, cos(theta) * radius);
			addVec3(vertices, cos(phi1) * sin(theta1) * radius, sin(phi1) * sin(theta1) * radius, cos(theta1) * radius);

			addVec3(normals, cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
			addVec3(normals, cos(phi1) * sin(theta), sin(phi1) * sin(theta), cos(theta));
			addVec3(normals, cos(phi1) * sin(theta1), sin(phi1) * sin(theta1), cos(theta1));

			addVec2(texcoords, (float)j / N, 1.0 - (float)i / M);
			addVec2(texcoords, (float)(j + 1) / N, 1.0 - (float)i / M);
			addVec2(texcoords, (float)(j + 1) / N, 1.0 - (float)(i + 1) / M);						

			_sphereNumTris++;

			//Второй треугольник, образующий квад
			addVec3(vertices, cos(phi) * sin(theta) * radius, sin(phi) * sin(theta) * radius, cos(theta) * radius);
			addVec3(vertices, cos(phi1) * sin(theta1) * radius, sin(phi1) * sin(theta1) * radius, cos(theta1) * radius);
			addVec3(vertices, cos(phi) * sin(theta1) * radius, sin(phi) * sin(theta1) * radius, cos(theta1) * radius);			

			addVec3(normals, cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
			addVec3(normals, cos(phi1) * sin(theta1), sin(phi1) * sin(theta1), cos(theta1));
			addVec3(normals, cos(phi) * sin(theta1), sin(phi) * sin(theta1), cos(theta1));			

			addVec2(texcoords, (float)j / N, 1.0 - (float)i / M);
			addVec2(texcoords, (float)(j + 1) / N, 1.0 - (float)(i + 1) / M);
			addVec2(texcoords, (float)j / N, 1.0 - (float)(i + 1) / M);			

			_sphereNumTris++;
		}
	}

	vertices.insert(vertices.end(), normals.begin(), normals.end());
	vertices.insert(vertices.end(), texcoords.begin(), texcoords.end());

	unsigned int vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	_sphereVao = 0;
	glGenVertexArrays(1, &_sphereVao);
	glBindVertexArray(_sphereVao);
	glEnableVertexAttribArray(0);	
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(_sphereNumTris * 3 * 3 * 4));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(_sphereNumTris * 3 * 3 * 4 * 2));
	
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
	addVec3(vertices, size, -size, size);
	addVec3(vertices, size, size, size);
	addVec3(vertices, size, size, -size);

	addVec4(colors, 1.0, 0.0, 0.0, 1.0);
	addVec4(colors, 1.0, 0.0, 0.0, 1.0);
	addVec4(colors, 1.0, 0.0, 0.0, 1.0);

	addVec3(normals, 1.0, 0.0, 0.0);
	addVec3(normals, 1.0, 0.0, 0.0);
	addVec3(normals, 1.0, 0.0, 0.0);

	//front 2
	addVec3(vertices, size, -size, size);
	addVec3(vertices, size, size, -size);
	addVec3(vertices, size, -size, -size);

	addVec4(colors, 1.0, 0.0, 0.0, 1.0);
	addVec4(colors, 1.0, 0.0, 0.0, 1.0);
	addVec4(colors, 1.0, 0.0, 0.0, 1.0);

	addVec3(normals, 1.0, 0.0, 0.0);
	addVec3(normals, 1.0, 0.0, 0.0);
	addVec3(normals, 1.0, 0.0, 0.0);

	//left 1
	addVec3(vertices, -size, -size, size);
	addVec3(vertices, size, -size, size);
	addVec3(vertices, size, -size, -size);

	addVec4(colors, 1.0, 1.0, 0.0, 1.0);
	addVec4(colors, 1.0, 1.0, 0.0, 1.0);
	addVec4(colors, 1.0, 1.0, 0.0, 1.0);	

	addVec3(normals, 0.0, -1.0, 0.0);
	addVec3(normals, 0.0, -1.0, 0.0);
	addVec3(normals, 0.0, -1.0, 0.0);	

	//left 2
	addVec3(vertices, -size, -size, size);
	addVec3(vertices, size, -size, -size);
	addVec3(vertices, -size, -size, -size);

	addVec4(colors, 1.0, 1.0, 0.0, 1.0);
	addVec4(colors, 1.0, 1.0, 0.0, 1.0);
	addVec4(colors, 1.0, 1.0, 0.0, 1.0);

	addVec3(normals, 0.0, -1.0, 0.0);
	addVec3(normals, 0.0, -1.0, 0.0);
	addVec3(normals, 0.0, -1.0, 0.0);

	//top 1
	addVec3(vertices, -size, size, size);
	addVec3(vertices, size, size, size);
	addVec3(vertices, size, -size, size);

	addVec4(colors, 0.0, 1.0, 0.0, 1.0);
	addVec4(colors, 0.0, 1.0, 0.0, 1.0);
	addVec4(colors, 0.0, 1.0, 0.0, 1.0);

	addVec3(normals, 0.0, 0.0, 1.0);
	addVec3(normals, 0.0, 0.0, 1.0);
	addVec3(normals, 0.0, 0.0, 1.0);

	//top 2
	addVec3(vertices, -size, size, size);
	addVec3(vertices, -size, -size, size);
	addVec3(vertices, size, -size, size);

	addVec4(colors, 0.0, 1.0, 0.0, 1.0);
	addVec4(colors, 0.0, 1.0, 0.0, 1.0);
	addVec4(colors, 0.0, 1.0, 0.0, 1.0);

	addVec3(normals, 0.0, 0.0, 1.0);
	addVec3(normals, 0.0, 0.0, 1.0);
	addVec3(normals, 0.0, 0.0, 1.0);

	//back 1
	addVec3(vertices, -size, -size, size);
	addVec3(vertices, -size, size, -size);
	addVec3(vertices, -size, size, size);

	addVec4(colors, 0.0, 0.0, 1.0, 1.0);
	addVec4(colors, 0.0, 0.0, 1.0, 1.0);
	addVec4(colors, 0.0, 0.0, 1.0, 1.0);

	addVec3(normals, -1.0, 0.0, 0.0);
	addVec3(normals, -1.0, 0.0, 0.0);
	addVec3(normals, -1.0, 0.0, 0.0);

	//back 2
	addVec3(vertices, -size, -size, size);
	addVec3(vertices, -size, -size, -size);
	addVec3(vertices, -size, size, -size);

	addVec4(colors, 0.0, 0.0, 1.0, 1.0);
	addVec4(colors, 0.0, 0.0, 1.0, 1.0);
	addVec4(colors, 0.0, 0.0, 1.0, 1.0);

	addVec3(normals, -1.0, 0.0, 0.0);
	addVec3(normals, -1.0, 0.0, 0.0);
	addVec3(normals, -1.0, 0.0, 0.0);

	//right 1
	addVec3(vertices, -size, size, size);
	addVec3(vertices, size, size, -size);
	addVec3(vertices, size, size, size);

	addVec4(colors, 0.0, 1.0, 1.0, 1.0);
	addVec4(colors, 0.0, 1.0, 1.0, 1.0);
	addVec4(colors, 0.0, 1.0, 1.0, 1.0);

	addVec3(normals, 0.0, 1.0, 0.0);
	addVec3(normals, 0.0, 1.0, 0.0);
	addVec3(normals, 0.0, 1.0, 0.0);

	//right 2
	addVec3(vertices, -size, size, size);
	addVec3(vertices, -size, size, -size);
	addVec3(vertices, +size, size, -size);

	addVec4(colors, 0.0, 1.0, 1.0, 1.0);
	addVec4(colors, 0.0, 1.0, 1.0, 1.0);
	addVec4(colors, 0.0, 1.0, 1.0, 1.0);

	addVec3(normals, 0.0, 1.0, 0.0);
	addVec3(normals, 0.0, 1.0, 0.0);
	addVec3(normals, 0.0, 1.0, 0.0);

	//bottom 1
	addVec3(vertices, -size, size, -size);
	addVec3(vertices, size, -size, -size);
	addVec3(vertices, size, size, -size);

	addVec4(colors, 1.0, 0.0, 1.0, 1.0);
	addVec4(colors, 1.0, 0.0, 1.0, 1.0);
	addVec4(colors, 1.0, 0.0, 1.0, 1.0);

	addVec3(normals, 0.0, 0.0, -1.0);
	addVec3(normals, 0.0, 0.0, -1.0);
	addVec3(normals, 0.0, 0.0, -1.0);

	//bottom 2
	addVec3(vertices, -size, size, -size);
	addVec3(vertices, size, -size, -size);
	addVec3(vertices, -size, -size, -size);

	addVec4(colors, 1.0, 0.0, 1.0, 1.0);
	addVec4(colors, 1.0, 0.0, 1.0, 1.0);
	addVec4(colors, 1.0, 0.0, 1.0, 1.0);

	addVec3(normals, 0.0, 0.0, -1.0);
	addVec3(normals, 0.0, 0.0, -1.0);
	addVec3(normals, 0.0, 0.0, -1.0);

	vertices.insert(vertices.end(), normals.begin(), normals.end());

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
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(_cubeNumTris * 3 * 3 * 4));

	glBindVertexArray(0);

	_cubeModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
}

void Application::makeShaders()
{
	/*std::string vertFilename = "shaders5/shader.vert";
	std::string fragFilename = "shaders5/shader.frag";*/

	std::string vertFilename = "shaders5/texture_without_lighting.vert";
	std::string fragFilename = "shaders5/texture_without_lighting.frag";
	
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
	//Инициализация uniform-переменных для преобразования координат

	_timeUniform = glGetUniformLocation(_shaderProgram, "time");
	_modelMatrixUniform = glGetUniformLocation(_shaderProgram, "modelMatrix");
	_viewMatrixUniform = glGetUniformLocation(_shaderProgram, "viewMatrix");
	_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");
	_normalToCameraMatrixUniform = glGetUniformLocation(_shaderProgram, "normalToCameraMatrix");
	
	_viewMatrix = glm::lookAt(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	_projMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.f);

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

	//Инициализация значений переменных освщения
	_lightDir = glm::vec4(0.0f, 1.0f, 0.8f, 0.0f);
	_lightPos = glm::vec4(0.0f, 1.0f, 0.8f, 1.0f);
	_ambientColor = glm::vec3(0.2, 0.2, 0.2);
	_diffuseColor = glm::vec3(0.8, 0.8, 0.8);
	_specularColor = glm::vec3(0.25, 0.25, 0.25);
		
	_sphereShininess = 100.0f;
	_cubeShininess = 100.0f;

	_sphereMaterial = glm::vec3(1.0, 0.0, 0.0);
	_cubeMaterial = glm::vec3(0.0, 1.0, 0.0);

	_attenuation = 1.0f;

	//Текстура
	_texUniform = glGetUniformLocation(_shaderProgram, "diffuseTex");
	glUseProgram(_shaderProgram);
	glUniform1i(_texUniform, 0);
}

void Application::drawImplementation()
{
	glUseProgram(_shaderProgram);

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

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, _texId);
	
	//====== Сфера ======
	//Копирование на видеокарту значений uniform-пемеренных для сферы
	_normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * _sphereModelMatrix)));
	glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(_normalToCameraMatrix));
	
	glUniform3fv(_materialUniform, 1, glm::value_ptr(_sphereMaterial));
	glUniform1f(_shininessUniform, _sphereShininess);

	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_sphereModelMatrix));	

	glBindVertexArray(_sphereVao); //Подключаем VertexArray для сферы
	glDrawArrays(GL_TRIANGLES, 0, _sphereNumTris * 3); //Рисуем сферу


	//====== Куб ======
	//Копирование на видеокарту значений uniform-пемеренных для куба
	_normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * _cubeModelMatrix)));
	glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(_normalToCameraMatrix));

	glUniform3fv(_materialUniform, 1, glm::value_ptr(_cubeMaterial));
	glUniform1f(_shininessUniform, _cubeShininess);

	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_cubeModelMatrix));

	glBindVertexArray(_cubeVao); //Подключаем VertexArray для куба
	glDrawArrays(GL_TRIANGLES, 0, _cubeNumTris * 3); //Рисуем куб
}