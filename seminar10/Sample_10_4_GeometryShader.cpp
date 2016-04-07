#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>
#include <Texture.hpp>

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

/**
Инстансинг
*/
class SampleApplication : public Application
{
public:
    //Идентификатор шейдерной программы    
    ShaderProgramPtr _particleShader;

    TexturePtr _particleTex;

    GLuint _sampler;
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

    void makeScene() override
    {
        Application::makeScene();

        _oldTime = 0.0;
        _deltaTime = 0.0;
        _fps = 0.0;

        //=========================================================
        //Создание и загрузка мешей		

        //=========================================================
        //Инициализация шейдеров

        _particleShader = std::make_shared<ShaderProgram>();

        ShaderPtr vs = std::make_shared<Shader>(GL_VERTEX_SHADER);
        vs->createFromFile("shaders10/particleWithGeomShader.vert");
        _particleShader->attachShader(vs);

        ShaderPtr gs = std::make_shared<Shader>(GL_GEOMETRY_SHADER);
        gs->createFromFile("shaders10/particleWithGeomShader.geom");
        _particleShader->attachShader(gs);

        ShaderPtr fs = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
        fs->createFromFile("shaders10/particleWithGeomShader.frag");
        _particleShader->attachShader(fs);
        
        _particleShader->linkProgram();

        //=========================================================
        //Загрузка и создание текстур
        _particleTex = loadTexture("images/particle.png", false, true);

        //=========================================================
        //Инициализация сэмплера, объекта, который хранит параметры чтения из текстуры
        glGenSamplers(1, &_sampler);
        glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

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

        _deltaTime = glfwGetTime() - _oldTime;
        _oldTime = glfwGetTime();

        computeFPS();

        updateParticles();
    }

    void draw() override
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

    void drawParticles(const ShaderProgramPtr& shader)
    {
        shader->use();

        //Загружаем на видеокарту значения юниформ-переменных
        shader->setMat4Uniform("modelMatrix", glm::mat4(1.0f));
        shader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        shader->setMat4Uniform("projectionMatrix", _camera.projMatrix);
        shader->setFloatUniform("time", (float)glfwGetTime());

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0
        _particleTex->bind();
        glBindSampler(0, _sampler);
        shader->setIntUniform("tex", 0);

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