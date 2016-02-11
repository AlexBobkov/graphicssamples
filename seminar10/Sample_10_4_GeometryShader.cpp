#include "Application.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Framebuffer.h"

#include <Mesh.hpp>

#include <iostream>
#include <sstream>
#include <vector>
#include <deque>

const int numParticles = 1000;
const float emitterSize = 1.0;
const float lifeTime = 3.0;

struct Particle
{
    glm::vec3 position;
    glm::vec3 velocity;
    float startTime;
};

struct LightInfo
{
	glm::vec3 position; //Будем здесь хранить координаты в мировой системе координат, а при копировании в юниформ-переменную конвертировать в систему виртуальной камеры
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

float frand()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

void getColorFromLinearPalette(float value, float& r, float& g, float& b);

/**
Инстансинг
*/
class SampleApplication : public Application
{
public:
	Mesh cube;
	Mesh sphere;
	Mesh bunny;
	Mesh ground;
	Mesh backgroundCube;

    Mesh teapot;
    Mesh teapotArray;

	Mesh quad;

	Mesh marker; //Меш - маркер для источника света

	//Идентификатор шейдерной программы
	ShaderProgram _commonShader;
	ShaderProgram _markerShader;
	ShaderProgram _skyboxShader;
	ShaderProgram _quadDepthShader;
    ShaderProgram _quadColorShader;

    ShaderProgram _particleShader;

	//Переменные для управления положением одного источника света
	float _lr;
	float _phi;
	float _theta;
		
	LightInfo _light;
    CameraInfo _lightCamera;
    
	GLuint _worldTexId;
	GLuint _brickTexId;
	GLuint _grassTexId;
	GLuint _chessTexId;
	GLuint _myTexId;
	GLuint _cubeTexId;    

    GLuint _particleTexId;

	GLuint _sampler;
	GLuint _cubeTexSampler;
    GLuint _depthSampler;
    
    float _oldTime;    
    float _deltaTime;
    float _fps;
    std::deque<float> _fpsData;

    std::vector<Particle> _particles;
    std::vector<float> _particlePositions;    
    std::vector<float> _particleTimes;

    GLuint _particlePosVbo;
    GLuint _particleTimeVbo;
    GLuint _particleVao;
    
	virtual void makeScene()
	{
		Application::makeScene();

        _oldTime = 0.0;
        _deltaTime = 0.0;
        _fps = 0.0;

		//=========================================================
		//Создание и загрузка мешей		

		cube.makeCube(0.5);
		cube.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f));

		sphere.makeSphere(0.5, 100);
		sphere.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f));

		bunny.loadFromFile("models/bunny.obj");
		bunny.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        teapot.loadFromFile("models/teapot.obj");

		ground.makeGroundPlane(5.0f, 2.0f);

		marker.makeSphere(0.1f);

		backgroundCube.makeCube(10.0f);

		quad.makeScreenAlignedQuad();

		//=========================================================
		//Инициализация шейдеров

		_commonShader.createProgram("shaders6/common.vert", "shaders6/common.frag");
		_markerShader.createProgram("shaders4/marker.vert", "shaders4/marker.frag");
		_skyboxShader.createProgram("shaders6/skybox.vert", "shaders6/skybox.frag");
		_quadDepthShader.createProgram("shaders7/quadDepth.vert", "shaders7/quadDepth.frag");
        _quadColorShader.createProgram("shaders7/quadColor.vert", "shaders7/quadColor.frag");
        _particleShader.createProgram("shaders10/particleWithGeomShader.vert", "shaders10/particleWithGeomShader.geom", "shaders10/particleWithGeomShader.frag");

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
        _particleTexId = Texture::loadTexture("images/particle.png", false, true);

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

        GLfloat border[] = { 1.0f, 0.0f, 0.0f, 1.0f };

        glGenSamplers(1, &_depthSampler);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glSamplerParameterfv(_depthSampler, GL_TEXTURE_BORDER_COLOR, border);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

        //=========================================================

        srand((int)(glfwGetTime() * 1000));

        for (unsigned int i = 0; i < numParticles; i++)
        {
            _particlePositions.push_back(0.0f);
            _particlePositions.push_back(0.0f);
            _particlePositions.push_back(0.0f);

            _particleTimes.push_back(0.0f);
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

	virtual void initGUI()
	{
		Application::initGUI();

        TwAddVarRO(_bar, "FPS", TW_TYPE_FLOAT, &_fps, "");
		TwAddVarRW(_bar, "r", TW_TYPE_FLOAT, &_lr, "group=Light step=0.01 min=0.1 max=100.0");
		TwAddVarRW(_bar, "phi", TW_TYPE_FLOAT, &_phi, "group=Light step=0.01 min=0.0 max=6.28");
		TwAddVarRW(_bar, "theta", TW_TYPE_FLOAT, &_theta, "group=Light step=0.01 min=-1.57 max=1.57");
		TwAddVarRW(_bar, "La", TW_TYPE_COLOR3F, &_light.ambient, "group=Light label='ambient'");
		TwAddVarRW(_bar, "Ld", TW_TYPE_COLOR3F, &_light.diffuse, "group=Light label='diffuse'");
		TwAddVarRW(_bar, "Ls", TW_TYPE_COLOR3F, &_light.specular, "group=Light label='specular'");
	}

    virtual void handleKey(int key, int scancode, int action, int mods)
    {
        Application::handleKey(key, scancode, action, mods);

        if (action == GLFW_PRESS)
        {
            
        }
    }

    void computeFPS()
    {
        _fpsData.push_back(1 / _deltaTime);
        while (_fpsData.size() > 10)
        {
            _fpsData.pop_front();
        }

        _fps = 0.0;
        if (_fpsData.size() > 0)
        {
            for (unsigned int i = 0; i < _fpsData.size(); i++)
            {
                _fps += _fpsData[i];
            }
            _fps /= _fpsData.size();
            _fps = floor(_fps);
        }
    }

    Particle makeNewParticle()
    {
        float r = frand() * emitterSize;
        float theta = frand() * 0.1;
        float phi = frand() * 2.0 * glm::pi<float>();

        Particle p;
        p.startTime = glfwGetTime();
        p.position = glm::vec3(cos(phi) * r, sin(phi) * r, -2.0);
        p.velocity = glm::vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta)) * 5.0f;

        return p;
    }

    void updateParticles()
    {   
        if (_particles.size() < numParticles)
        {
            int planningCount = numParticles * glfwGetTime() / lifeTime;
            int addParticlesCount = glm::min<int>(numParticles - _particles.size(), planningCount - _particles.size());

            for (unsigned int i = 0; i < addParticlesCount; i++)
            {
                Particle p = makeNewParticle();
                p.startTime = frand() * lifeTime;
                _particles.push_back(p);
            }
        }

        float curTime = glfwGetTime();
        float animationDeltaTime = glm::min(_deltaTime, 0.03f);

        for (unsigned int i = 0; i < _particles.size(); i++)
        {
            _particles[i].velocity += glm::vec3(0.0, 0.0, -1.0) * animationDeltaTime; //Гравитация
            _particles[i].position += _particles[i].velocity * animationDeltaTime;

            if (curTime - _particles[i].startTime > lifeTime)
            {
                _particles[i] = makeNewParticle();
            }

            if (_particles[i].position.z < 0.0)
            {
                //_particles[i] = makeNewParticle();
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

    void update()
    {
        Application::update();

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        _lightCamera.viewMatrix = glm::lookAt(_light.position, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        _lightCamera.projMatrix = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 30.f);

        _deltaTime = glfwGetTime() - _oldTime;
        _oldTime = glfwGetTime();

        computeFPS();

        updateParticles();
    }

    virtual void draw()
    {
        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        drawParticles(_particleShader);


        //Отсоединяем сэмплер и шейдерную программу
        glBindSampler(0, 0);
        glUseProgram(0);
    }

    void drawParticles(const ShaderProgram& shader)
    {
        shader.use();

        //Загружаем на видеокарту значения юниформ-переменных
        shader.setMat4Uniform("modelMatrix", glm::mat4(1.0f));
        shader.setMat4Uniform("viewMatrix", _camera.viewMatrix);
        shader.setMat4Uniform("projectionMatrix", _camera.projMatrix);
        shader.setFloatUniform("time", (float)glfwGetTime());
        
        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0
        glBindTexture(GL_TEXTURE_2D, _particleTexId);
        glBindSampler(0, _sampler);
        shader.setIntUniform("tex", 0);
        
        glDisable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindVertexArray(_particleVao); //Подключаем VertexArray
        glDrawArrays(GL_POINTS, 0, _particles.size()); //Рисуем		

        glDisable(GL_BLEND);

        glEnable(GL_DEPTH_TEST);
    }
};

int main()
{
	SampleApplication app;
	app.start();

	return 0;
}