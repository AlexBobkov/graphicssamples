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
    ShaderProgramPtr _transformFeedbackPass1Shader;
    ShaderProgramPtr _transformFeedbackPass2Shader;

    TexturePtr _particleTex;

    GLuint _sampler;
    GLuint _depthSampler;

    float _oldTime;
    float _deltaTime;
    float _fps;
    std::deque<float> _fpsData;

    std::vector<Particle> _particles;
    std::vector<float> _particlePositions;
    std::vector<float> _particleVelocities;
    std::vector<float> _particleTimes;

    GLuint _particleVaoTF[2];
    GLuint _particlePosVboTF[2];
    GLuint _particleVelVboTF[2];
    GLuint _particleTimeVboTF[2];
    GLuint _TF[2];

    int _tfIndex;
    bool _firstTime;

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
        
        _transformFeedbackPass1Shader = std::make_shared<ShaderProgram>();
        
        ShaderPtr vs = std::make_shared<Shader>(GL_VERTEX_SHADER);
        vs->createFromFile("shaders10/transformFeedbackPass1.vert");
        _transformFeedbackPass1Shader->attachShader(vs);
        
        ShaderPtr fs = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
        fs->createFromFile("shaders10/particle.frag");
        _transformFeedbackPass1Shader->attachShader(fs);

        std::vector<std::string> attribs;
        attribs.push_back("position");
        attribs.push_back("velocity");
        attribs.push_back("particleTime");

        GLchar** attribStrings = new GLchar*[attribs.size()];
        for (unsigned int i = 0; i < attribs.size(); i++)
        {
            attribStrings[i] = const_cast<GLchar*>(attribs[i].c_str());
        }
        glTransformFeedbackVaryings(_transformFeedbackPass1Shader->id(), attribs.size(), attribStrings, GL_SEPARATE_ATTRIBS);
        
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
            Particle p;
            p.position = glm::vec3((frand() - 0.5) * emitterSize, (frand() - 0.5) * emitterSize, frand() * 5.0);
            p.velocity = glm::vec3(frand() * 0.01, frand() * 0.01, 0.0);
            p.startTime = frand() * lifeTime;
            _particles.push_back(p);
        }

        for (unsigned int i = 0; i < numParticles; i++)
        {
            _particlePositions.push_back(_particles[i].position.x);
            _particlePositions.push_back(_particles[i].position.y);
            _particlePositions.push_back(_particles[i].position.z);

            _particleVelocities.push_back(_particles[i].velocity.x);
            _particleVelocities.push_back(_particles[i].velocity.y);
            _particleVelocities.push_back(_particles[i].velocity.z);

            _particleTimes.push_back(_particles[i].startTime);
        }

        glGenTransformFeedbacks(2, _TF);
        glGenVertexArrays(2, _particleVaoTF);
        glGenBuffers(2, _particlePosVboTF);
        glGenBuffers(2, _particleVelVboTF);
        glGenBuffers(2, _particleTimeVboTF);

        for (unsigned int i = 0; i < 2; i++)
        {
            glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, _TF[i]);

            glBindVertexArray(_particleVaoTF[i]);

            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, _particlePosVboTF[i]);
            glBufferData(GL_ARRAY_BUFFER, _particlePositions.size() * sizeof(float), _particlePositions.data(), GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, _particlePosVboTF[i]);

            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, _particleVelVboTF[i]);
            glBufferData(GL_ARRAY_BUFFER, _particleVelocities.size() * sizeof(float), _particleVelocities.data(), GL_DYNAMIC_DRAW);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, _particleVelVboTF[i]);

            glEnableVertexAttribArray(2);
            glBindBuffer(GL_ARRAY_BUFFER, _particleTimeVboTF[i]);
            glBufferData(GL_ARRAY_BUFFER, _particleTimes.size() * sizeof(float), _particleTimes.data(), GL_DYNAMIC_DRAW);
            glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, NULL);
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, _particleTimeVboTF[i]);
        }

        glBindVertexArray(0);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

        _tfIndex = 0;
        _firstTime = true;
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

    void update()
    {
        Application::update();

        _deltaTime = glfwGetTime() - _oldTime;
        _oldTime = glfwGetTime();

        computeFPS();
    }

    void draw() override
    {
        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        drawParticles();


        //Отсоединяем сэмплер и шейдерную программу
        glBindSampler(0, 0);
        glUseProgram(0);
    }

    void drawParticles()
    {
        int curVB = 1 - _tfIndex;

        //=========================================================

        _transformFeedbackPass1Shader->use();
        _transformFeedbackPass1Shader->setFloatUniform("deltaTime", _deltaTime);

        glEnable(GL_RASTERIZER_DISCARD);

        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, _TF[_tfIndex]);
        glBindVertexArray(_particleVaoTF[curVB]);

        glBeginTransformFeedback(GL_POINTS);

        if (_firstTime)
        {
            glDrawArrays(GL_POINTS, 0, _particlePositions.size());
            _firstTime = false;
        }
        else
        {
            glDrawTransformFeedback(GL_POINTS, _TF[curVB]);
        }

        glEndTransformFeedback();

        glDisable(GL_RASTERIZER_DISCARD);

        //=========================================================

        _transformFeedbackPass2Shader->use();

        _transformFeedbackPass2Shader->setMat4Uniform("modelMatrix", glm::mat4(1.0f));
        _transformFeedbackPass2Shader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        _transformFeedbackPass2Shader->setMat4Uniform("projectionMatrix", _camera.projMatrix);
        _transformFeedbackPass2Shader->setFloatUniform("time", (float)glfwGetTime());

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0
        _particleTex->bind();
        glBindSampler(0, _sampler);
        _transformFeedbackPass2Shader->setIntUniform("tex", 0);

        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_POINT_SPRITE);

        glDisable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


        glBindVertexArray(_particleVaoTF[_tfIndex]);
        glDrawTransformFeedback(GL_POINTS, _TF[_tfIndex]); //Рисуем здесь!


        glDisable(GL_BLEND);

        glEnable(GL_DEPTH_TEST);

        _tfIndex = 1 - _tfIndex;
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}