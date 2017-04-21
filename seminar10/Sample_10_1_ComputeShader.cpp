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
    const int numParticles = 100000;
    const float emitterSize = 20.0;
    const float maxSpeed = 1.0f;

    float frand()
    {
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }
}

/**
Пример расчета движения частиц с помощью вычислительного шейдера
*/
class SampleApplication : public Application
{
public:
    ShaderProgramPtr _particleShader;
    ShaderProgramPtr _computeShader;

    TexturePtr _particleTex;

    GLuint _sampler;

    float _oldTime = 0.0f;
    float _deltaTime = 0.0f;

    std::vector<glm::vec4> _particlePositions;
    std::vector<glm::vec4> _particleVelocities;

    glm::vec3 _attractor1Pos = glm::vec3(-10.0, 0.0, 0.0);
    glm::vec3 _attractor2Pos = glm::vec3(10.0, 0.0, 0.0);

    float _attractor1Gravity = 50.0f;
    float _attractor2Gravity = 100.0f;

    GLuint _particlePosVbo;
    GLuint _particleVelVbo;
    GLuint _particleVao;

    void makeScene() override
    {
        Application::makeScene();

        GLint range[2];
        glGetIntegerv(GL_POINT_SIZE_RANGE, range);

        std::cout << "Point size range " << range[0] << " " << range[1] << std::endl;

        //=========================================================
        //Создание и загрузка мешей		

        //=========================================================
        //Инициализация шейдеров

        _particleShader = std::make_shared<ShaderProgram>("shaders10/particleWithComputeShader.vert", "shaders10/particleWithComputeShader.frag");

        _computeShader = std::make_shared<ShaderProgram>();

        ShaderPtr vs = std::make_shared<Shader>(GL_COMPUTE_SHADER);
        vs->createFromFile("shaders10/particleWithComputeShader.compute");
        _computeShader->attachShader(vs);

        _computeShader->linkProgram();

        //=========================================================
        //Загрузка и создание текстур
        _particleTex = loadTexture("images/particle.png");

        //=========================================================
        //Инициализация сэмплера, объекта, который хранит параметры чтения из текстуры
        glGenSamplers(1, &_sampler);
        glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        //=========================================================

        srand((int)(glfwGetTime() * 1000));

        _particlePositions.resize(numParticles, glm::vec4());
        _particleVelocities.resize(numParticles, glm::vec4());

        for (unsigned int i = 0; i < numParticles; i++)
        {
            _particlePositions[i] = glm::vec4((frand() - 0.5) * emitterSize, (frand() - 0.5) * emitterSize, (frand() - 0.5) * emitterSize, 1.0f);
            _particleVelocities[i] = glm::vec4((frand() - 0.5) * maxSpeed, (frand() - 0.5) * maxSpeed, (frand() - 0.5) * maxSpeed, 0.0f);
        }

        //--------------------------------

        _particlePosVbo = 0;
        glGenBuffers(1, &_particlePosVbo);
        glBindBuffer(GL_ARRAY_BUFFER, _particlePosVbo);
        glBufferData(GL_ARRAY_BUFFER, _particlePositions.size() * sizeof(float) * 4, _particlePositions.data(), GL_STREAM_DRAW);

        _particleVelVbo = 0;
        glGenBuffers(1, &_particleVelVbo);
        glBindBuffer(GL_ARRAY_BUFFER, _particleVelVbo);
        glBufferData(GL_ARRAY_BUFFER, _particleVelocities.size() * sizeof(float) * 4, _particleVelocities.data(), GL_STREAM_DRAW);

        //--------------------------------

        _particleVao = 0;
        glGenVertexArrays(1, &_particleVao);
        glBindVertexArray(_particleVao);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, _particlePosVbo);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);

        glBindVertexArray(0);

        //--------------------------------

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _particlePosVbo);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _particleVelVbo);
    }

    void updateGUI() override
    {
        Application::updateGUI();

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
        if (ImGui::Begin("MIPT OpenGL Sample", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("FPS %.1f", ImGui::GetIO().Framerate);

            ImGui::SliderFloat3("attractor1", &_attractor1Pos[0], -emitterSize, emitterSize);
            ImGui::SliderFloat("attractor1 gravity", &_attractor1Gravity, 0.0, 200.0f);
            ImGui::SliderFloat3("attractor2", &_attractor2Pos[0], -emitterSize, emitterSize);
            ImGui::SliderFloat("attractor2 gravity", &_attractor2Gravity, 0.0, 200.0f);
        }
        ImGui::End();
    }

    void update()
    {
        Application::update();

        float time = static_cast<float>(glfwGetTime());
        _deltaTime = time - _oldTime;
        _oldTime = time;

        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        //Выставим побольше far plane
        _camera.projMatrix = glm::perspective(glm::radians(45.0f), (float)width / height, 1.0f, 10000.f);
    }

    void draw() override
    {
        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //-------------------------

        updateParticles(_computeShader);
        drawParticles(_particleShader);

        //-------------------------

        //Отсоединяем сэмплер и шейдерную программу
        glBindSampler(0, 0);
        glUseProgram(0);
    }

    void updateParticles(const ShaderProgramPtr& shader)
    {
        shader->use();

        shader->setFloatUniform("time", static_cast<float>(glfwGetTime()));
        shader->setFloatUniform("deltaTime", _deltaTime);
        shader->setIntUniform("numParticles", numParticles);

        shader->setVec3Uniform("attractor1Pos", _attractor1Pos);
        shader->setFloatUniform("attractor1Gravity", _attractor1Gravity);

        shader->setVec3Uniform("attractor2Pos", _attractor2Pos);
        shader->setFloatUniform("attractor2Gravity", _attractor2Gravity);

        unsigned int posIndex = glGetProgramResourceIndex(shader->id(), GL_SHADER_STORAGE_BLOCK, "Positions");
        glShaderStorageBlockBinding(shader->id(), posIndex, 0); //0я точка привязки

        unsigned int velIndex = glGetProgramResourceIndex(shader->id(), GL_SHADER_STORAGE_BLOCK, "Velocities");
        glShaderStorageBlockBinding(shader->id(), velIndex, 1); //1я точка привязки

        glDispatchCompute(numParticles / 500, 1, 1); //500 - количество вызовов внутри одного рабочей группы (оно задается в шейдере)

        //Блокируем дальнейшее выполнение, пока не завершится запись в SSBO
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    void drawParticles(const ShaderProgramPtr& shader)
    {
        shader->use();

        //Загружаем на видеокарту значения юниформ-переменных
        shader->setMat4Uniform("modelMatrix", glm::mat4(1.0f));
        shader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        shader->setMat4Uniform("projectionMatrix", _camera.projMatrix);
        shader->setFloatUniform("time", static_cast<float>(glfwGetTime()));

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0        
        glBindSampler(0, _sampler);
        _particleTex->bind();
        shader->setIntUniform("tex", 0);

        glEnable(GL_PROGRAM_POINT_SIZE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDepthMask(false);

        glBindVertexArray(_particleVao); //Подключаем VertexArray
        glDrawArrays(GL_POINTS, 0, numParticles); //Рисуем		

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