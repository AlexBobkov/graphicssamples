#include <Application.hpp>
#include <LightInfo.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>
#include <Texture.hpp>

#include <iostream>
#include <sstream>
#include <vector>
#include <deque>

namespace
{
    const int numParticles = 1000;
    const float emitterSize = 1.0;
    const float lifeTime = 3.0;

    struct Particle
    {
        glm::vec3 position;
        glm::vec3 velocity;
        float startTime;
    };

    float frand()
    {
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }
}

/**
Пример с системой частиц с Transform Feedback
*/
class SampleApplication : public Application
{
public:
    //Идентификатор шейдерной программы    
    ShaderProgramPtr _transformFeedbackPass1Shader;
    ShaderProgramPtr _transformFeedbackPass2Shader;

    TexturePtr _particleTex;

    GLuint _sampler;

    float _oldTime;
    float _deltaTime;

    std::vector<Particle> _particles;
    std::vector<glm::vec3> _particlePositions;
    std::vector<glm::vec3> _particleVelocities;
    std::vector<float> _particleTimes;

    GLuint _particleVaoTF[2];
    GLuint _particlePosVboTF[2];
    GLuint _particleVelVboTF[2];
    GLuint _particleTimeVboTF[2];
    GLuint _TF[2];

    int _tfIndex;
    bool _firstTime;

    SampleApplication() :
        Application(),
        _oldTime(0.0),
        _deltaTime(0.0),
        _tfIndex(0),
        _firstTime(true)
    {
    }

    void makeScene() override
    {
        Application::makeScene();
        
        //=========================================================
        //Создание и загрузка мешей		

        //=========================================================
        //Инициализация шейдеров
        
        _transformFeedbackPass1Shader = std::make_shared<ShaderProgram>();
        
        ShaderPtr vs = std::make_shared<Shader>(GL_VERTEX_SHADER);
        vs->createFromFile("shaders10/transformFeedbackPass1.vert");
        _transformFeedbackPass1Shader->attachShader(vs);
        
        ShaderPtr fs = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
        fs->createFromFile("shaders10/particle.frag");
        _transformFeedbackPass1Shader->attachShader(fs);

        const char* attribs[] = { "position", "velocity", "particleTime" };
        glTransformFeedbackVaryings(_transformFeedbackPass1Shader->id(), 3, attribs, GL_SEPARATE_ATTRIBS);
        
        _transformFeedbackPass1Shader->linkProgram();

        //----------------------------

        _transformFeedbackPass2Shader = std::make_shared<ShaderProgram>();
        _transformFeedbackPass2Shader->createProgram("shaders10/transformFeedbackPass2.vert", "shaders10/particle.frag");

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

        //=========================================================

        srand((int)(glfwGetTime() * 1000));

        for (unsigned int i = 0; i < numParticles; i++)
        {
            Particle p;
            p.position = glm::vec3((frand() - 0.5) * emitterSize, (frand() - 0.5) * emitterSize, frand() * 5.0);
            p.velocity = glm::vec3(frand() * 0.01, frand() * 0.01, 0.0);
            p.startTime = frand() * lifeTime;
            _particles.push_back(p);
        }

        for (unsigned int i = 0; i < numParticles; i++)
        {
            _particlePositions.push_back(_particles[i].position);
            _particleVelocities.push_back(_particles[i].velocity);
            _particleTimes.push_back(_particles[i].startTime);
        }

        glGenTransformFeedbacks(2, _TF);
        glGenVertexArrays(2, _particleVaoTF);
        glGenBuffers(2, _particlePosVboTF);
        glGenBuffers(2, _particleVelVboTF);
        glGenBuffers(2, _particleTimeVboTF);

        for (unsigned int i = 0; i < 2; i++)
        {
            glBindVertexArray(_particleVaoTF[i]);

            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, _particlePosVboTF[i]);
            glBufferData(GL_ARRAY_BUFFER, _particlePositions.size() * sizeof(float) * 3, _particlePositions.data(), GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);            

            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, _particleVelVboTF[i]);
            glBufferData(GL_ARRAY_BUFFER, _particleVelocities.size() * sizeof(float) * 3, _particleVelocities.data(), GL_DYNAMIC_DRAW);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);            

            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, _particleTimeVboTF[i]);
            glBufferData(GL_ARRAY_BUFFER, _particleTimes.size() * sizeof(float), _particleTimes.data(), GL_DYNAMIC_DRAW);
            glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);

            //---------------------------

            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, _TF[i]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _particlePosVboTF[i]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, _particleVelVboTF[i]);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, _particleTimeVboTF[i]);
        }

        glBindVertexArray(0);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    }

    void update()
    {
        Application::update();

        float time = static_cast<float>(glfwGetTime());
        _deltaTime = time - _oldTime;
        _oldTime = time;
    }

    void draw() override
    {
        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //-----------------------------

        updateParticles();
        drawParticles();

        _tfIndex = 1 - _tfIndex;

        //-----------------------------

        //Отсоединяем сэмплер и шейдерную программу
        glBindSampler(0, 0);
        glUseProgram(0);
    }

    void updateParticles()
    {
        _transformFeedbackPass1Shader->use();
        _transformFeedbackPass1Shader->setFloatUniform("deltaTime", _deltaTime);

        glEnable(GL_RASTERIZER_DISCARD);

        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, _TF[_tfIndex]);

        glBeginTransformFeedback(GL_POINTS);

        if (_firstTime)
        {
            glBindVertexArray(_particleVaoTF[1 - _tfIndex]);
            glDrawArrays(GL_POINTS, 0, _particlePositions.size());
            _firstTime = false;
        }
        else
        {
            glBindVertexArray(_particleVaoTF[1 - _tfIndex]);
            glDrawTransformFeedback(GL_POINTS, _TF[1 - _tfIndex]);
        }

        glEndTransformFeedback();

        glDisable(GL_RASTERIZER_DISCARD);
    }

    void drawParticles()
    {
        _transformFeedbackPass2Shader->use();

        _transformFeedbackPass2Shader->setMat4Uniform("modelMatrix", glm::mat4(1.0f));
        _transformFeedbackPass2Shader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        _transformFeedbackPass2Shader->setMat4Uniform("projectionMatrix", _camera.projMatrix);
        _transformFeedbackPass2Shader->setFloatUniform("time", (float)glfwGetTime());

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0        
        glBindSampler(0, _sampler);
        _particleTex->bind();
        _transformFeedbackPass2Shader->setIntUniform("tex", 0);

        glEnable(GL_PROGRAM_POINT_SIZE);        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDepthMask(false);

        glBindVertexArray(_particleVaoTF[_tfIndex]);
        glDrawTransformFeedback(GL_POINTS, _TF[_tfIndex]); //Рисуем здесь!

        glDepthMask(true);

        glDisable(GL_BLEND);
        glDisable(GL_PROGRAM_POINT_SIZE);        
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}