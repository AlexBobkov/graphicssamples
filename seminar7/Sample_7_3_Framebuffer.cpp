#include "Application.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include <Mesh.hpp>

#include <iostream>
#include <sstream>
#include <vector>

struct LightInfo
{
	glm::vec3 position; //Будем здесь хранить координаты в мировой системе координат, а при копировании в юниформ-переменную конвертировать в систему виртуальной камеры
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

/**
Пример с рендерингом в текстуру
*/
class SampleApplication : public Application
{
public:
	Mesh cube;
	Mesh sphere;
	Mesh bunny;
	Mesh ground;
	Mesh backgroundCube;

	Mesh quad;

	Mesh marker; //Меш - маркер для источника света

	//Идентификатор шейдерной программы
	ShaderProgram _commonShader;
	ShaderProgram _markerShader;
	ShaderProgram _skyboxShader;
	ShaderProgram _quadShader;

	//Переменные для управления положением одного источника света
	float _lr;
	float _phi;
	float _theta;
		
	LightInfo _light;

	GLuint _worldTexId;
	GLuint _brickTexId;
	GLuint _grassTexId;
	GLuint _chessTexId;
	GLuint _myTexId;
	GLuint _cubeTexId;
    GLuint _renderTexId;

	GLuint _sampler;
	GLuint _cubeTexSampler;

    GLuint _framebufferId;
    unsigned int _fbWidth;
    unsigned int _fbHeight;

    CameraInfo _fbCamera;

    void initFramebuffer()
    {
        _fbWidth = 1024;
        _fbHeight = 1024;


        //Создаем фреймбуфер
        glGenFramebuffers(1, &_framebufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);


        //Создаем текстуру, куда будет осуществляться рендеринг	
        glGenTextures(1, &_renderTexId);
        glBindTexture(GL_TEXTURE_2D, _renderTexId);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _fbWidth, _fbHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

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

	virtual void makeScene()
	{
		Application::makeScene();

		//=========================================================
		//Создание и загрузка мешей		

		cube.makeCube(0.5);
		cube.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f));

		sphere.makeSphere(0.5, 100);
		sphere.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f));

		bunny.loadFromFile("models/bunny.obj");
		bunny.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		ground.makeGroundPlane(5.0f, 2.0f);

		marker.makeSphere(0.1);

		backgroundCube.makeCube(10.0f);

		quad.makeScreenAlignedQuad();

		//=========================================================
		//Инициализация шейдеров

		_commonShader.createProgram("shaders6/common.vert", "shaders6/common.frag");
		_markerShader.createProgram("shaders4/marker.vert", "shaders4/marker.frag");
		_skyboxShader.createProgram("shaders6/skybox.vert", "shaders6/skybox.frag");
		_quadShader.createProgram("shaders7/quadColor.vert", "shaders7/quadColor.frag");

		//=========================================================
		//Инициализация значений переменных освщения
		_lr = 10.0;
		_phi = 0.0f;
		_theta = 0.48f;

		_light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
		_light.ambient = glm::vec3(0.2, 0.2, 0.2);
		_light.diffuse = glm::vec3(0.8, 0.8, 0.8);
		_light.specular = glm::vec3(1.0, 1.0, 1.0);

		//=========================================================
		//Загрузка и создание текстур
		_worldTexId = Texture::loadTexture("images/earth_global.jpg");
		_brickTexId = Texture::loadTexture("images/brick.jpg");
		_grassTexId = Texture::loadTexture("images/grass.jpg");
		_chessTexId = Texture::loadTextureWithMipmaps("images/chess.dds");
		_myTexId = Texture::makeProceduralTexture();
		_cubeTexId = Texture::loadCubeTexture("images/cube");

		//=========================================================
		//Инициализация сэмплера, объекта, который хранит параметры чтения из текстуры
		glGenSamplers(1, &_sampler);
		glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glGenSamplers(1, &_cubeTexSampler);
		glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);		

        //=========================================================
        //Инициализация фреймбуфера и 2й виртуальной камеры - для рендеринга в текстуру

        initFramebuffer();

        _fbCamera.viewMatrix = glm::lookAt(glm::vec3(5.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f));
        _fbCamera.projMatrix = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.f);        
	}

	virtual void initGUI()
	{
		Application::initGUI();

		TwAddVarRW(_bar, "r", TW_TYPE_FLOAT, &_lr, "group=Light step=0.01 min=0.1 max=100.0");
		TwAddVarRW(_bar, "phi", TW_TYPE_FLOAT, &_phi, "group=Light step=0.01 min=0.0 max=6.28");
		TwAddVarRW(_bar, "theta", TW_TYPE_FLOAT, &_theta, "group=Light step=0.01 min=-1.57 max=1.57");
		TwAddVarRW(_bar, "La", TW_TYPE_COLOR3F, &_light.ambient, "group=Light label='ambient'");
		TwAddVarRW(_bar, "Ld", TW_TYPE_COLOR3F, &_light.diffuse, "group=Light label='diffuse'");
		TwAddVarRW(_bar, "Ls", TW_TYPE_COLOR3F, &_light.specular, "group=Light label='specular'");
	}

    virtual void draw()
    {
        drawToFramebuffer(_fbCamera);
        drawScene(_camera);
    }

    void drawToFramebuffer(const CameraInfo& camera)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);

        glViewport(0, 0, _fbWidth, _fbHeight);

        glClearColor(1, 1, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _commonShader.use();

        //Загружаем на видеокарту значения юниформ-переменных
        _commonShader.setMat4Uniform("viewMatrix", camera.viewMatrix);
        _commonShader.setMat4Uniform("projectionMatrix", camera.projMatrix);

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        glm::vec3 lightPosCamSpace = glm::vec3(camera.viewMatrix * glm::vec4(_light.position, 1.0));

        _commonShader.setVec3Uniform("light.pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
        _commonShader.setVec3Uniform("light.La", _light.ambient);
        _commonShader.setVec3Uniform("light.Ld", _light.diffuse);
        _commonShader.setVec3Uniform("light.Ls", _light.specular);

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0
        glBindTexture(GL_TEXTURE_2D, _brickTexId);
        glBindSampler(0, _sampler);
        _commonShader.setIntUniform("diffuseTex", 0);

        //Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
        {
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f * glm::sin(glfwGetTime())));

            _commonShader.setMat4Uniform("modelMatrix", modelMatrix);
            _commonShader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * modelMatrix))));

            bunny.draw();
        }

        //Отсоединяем сэмплер и шейдерную программу
        glBindSampler(0, 0);
        glUseProgram(0);

        //Отсоединяем фреймбуфер, чтобы теперь рендерить на экран
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void drawScene(const CameraInfo& camera)
	{
		//Получаем текущие размеры экрана и выставлям вьюпорт
		int width, height;
		glfwGetFramebufferSize(_window, &width, &height);

		glViewport(0, 0, width, height);

		//Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//====== РИСУЕМ ОСНОВНЫЕ ОБЪЕКТЫ СЦЕНЫ ======
		_commonShader.use();

		//Загружаем на видеокарту значения юниформ-переменных
		_commonShader.setMat4Uniform("viewMatrix", camera.viewMatrix);
		_commonShader.setMat4Uniform("projectionMatrix", camera.projMatrix);

		_light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
		glm::vec3 lightPosCamSpace = glm::vec3(camera.viewMatrix * glm::vec4(_light.position, 1.0));

		_commonShader.setVec3Uniform("light.pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
		_commonShader.setVec3Uniform("light.La", _light.ambient);
		_commonShader.setVec3Uniform("light.Ld", _light.diffuse);
		_commonShader.setVec3Uniform("light.Ls", _light.specular);

		glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0
		glBindTexture(GL_TEXTURE_2D, _renderTexId);
		glBindSampler(0, _sampler);
		_commonShader.setIntUniform("diffuseTex", 0);

		//Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
		{
			_commonShader.setMat4Uniform("modelMatrix", cube.modelMatrix());
			_commonShader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * cube.modelMatrix()))));

			cube.draw();
		}

		{
			_commonShader.setMat4Uniform("modelMatrix", sphere.modelMatrix());
			_commonShader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * sphere.modelMatrix()))));

			sphere.draw();
		}

		{
			_commonShader.setMat4Uniform("modelMatrix", bunny.modelMatrix());
			_commonShader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * bunny.modelMatrix()))));

			bunny.draw();
		}

		{
			_commonShader.setMat4Uniform("modelMatrix", ground.modelMatrix());
			_commonShader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * ground.modelMatrix()))));

			ground.draw();
		}

		//Рисуем маркеры для всех источников света		
		{
			_markerShader.use();

			_markerShader.setMat4Uniform("mvpMatrix", camera.projMatrix * camera.viewMatrix * glm::translate(glm::mat4(1.0f), _light.position));
            _markerShader.setVec4Uniform("color", glm::vec4(_light.diffuse, 1.0f));
			marker.draw();
		}

		//Отсоединяем сэмплер и шейдерную программу
		glBindSampler(0, 0);
		glUseProgram(0);
	}
};

int main()
{
	SampleApplication app;
	app.start();

	return 0;
}