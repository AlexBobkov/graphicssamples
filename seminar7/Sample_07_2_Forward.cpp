#include <Application.hpp>
#include <LightInfo.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>
#include <Texture.hpp>

#include <iostream>
#include <sstream>
#include <vector>

namespace
{
    float frand()
    {
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }

    //Удобная функция для вычисления цвета из линейной палитры от синего до красного
    glm::vec3 getColorFromLinearPalette(float value)
    {
        if (value < 0.25f)
        {
            return glm::vec3(0.0f, value * 4.0f, 1.0f);
        }
        else if (value < 0.5f)
        {
            return glm::vec3(0.0f, 1.0f, (0.5f - value) * 4.0f);
        }
        else if (value < 0.75f)
        {
            return glm::vec3((value - 0.5f) * 4.0f, 1.0f, 0.0f);
        }
        else
        {
            return glm::vec3(1.0f, (1.0f - value) * 4.0f, 0.0f);
        }
    }
}

/**
Пример с отложенным рендерингом
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _sphere;
    MeshPtr _bunny;
    MeshPtr _ground;

    MeshPtr _marker; //Маркер для источника света

    //Идентификатор шейдерной программы
    ShaderProgramPtr _prepassShader;
    ShaderProgramPtr _mainShader;
    ShaderProgramPtr _markerShader;

    //Переменные для управления положением одного источника света
    float _lr = 10.0;
    float _phi = 0.0;
    float _theta = 0.48;

    float _attenuation0 = 1.0;
    float _attenuation1 = 0.0;
    float _attenuation2 = 0.05;

    LightInfo _light;

    TexturePtr _brickTex;

    GLuint _sampler;

    int _Npositions = 100;
    int _Ncurrent = 0;
    std::vector<glm::vec3> _positions;

    int _Klights = 100;
    int _Kcurrent = 0;
    std::vector<LightInfo> _lights;

    void makeScene() override
    {
        Application::makeScene();

        //=========================================================
        //Создание и загрузка мешей		

        _cube = makeCube(0.5f);
        _cube->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f)));

        _sphere = makeSphere(0.5f);
        _sphere->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f)));

        _bunny = loadFromFile("models/bunny.obj");
        _bunny->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

        _ground = makeGroundPlane(10.0f, 2.0f);

        _marker = makeSphere(0.1f);

        //=========================================================
        //Инициализация шейдеров

        _prepassShader = std::make_shared<ShaderProgram>("shaders7/forwardLightingPrepass.vert", "shaders7/forwardLightingPrepass.frag");
        _mainShader = std::make_shared<ShaderProgram>("shaders7/forwardLighting.vert", "shaders7/forwardLighting.frag");
        _markerShader = std::make_shared<ShaderProgram>("shaders/marker.vert", "shaders/marker.frag");

        //=========================================================
        //Инициализация значений переменных освщения
        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * _lr;
        _light.ambient = glm::vec3(0.2, 0.2, 0.2);
        _light.diffuse = glm::vec3(0.8, 0.8, 0.8);
        _light.specular = glm::vec3(1.0, 1.0, 1.0);
        _light.attenuation0 = _attenuation0;
        _light.attenuation1 = _attenuation1;
        _light.attenuation2 = _attenuation2;

        //=========================================================
        //Загрузка и создание текстур
        _brickTex = loadTexture("images/brick.jpg");

        //=========================================================
        //Инициализация сэмплера, объекта, который хранит параметры чтения из текстуры
        glGenSamplers(1, &_sampler);
        glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

        //=========================================================
        srand((int)(glfwGetTime() * 1000));

        float size = 20.0f;
        for (int i = 0; i < _Npositions; i++)
        {
            _positions.push_back(glm::vec3(frand() * size - 0.5 * size, frand() * size - 0.5 * size, frand() * 1.0));
        }

        //=========================================================
        size = 30.0f;
        for (int i = 0; i < _Klights; i++)
        {
            LightInfo light;

            glm::vec3 color = getColorFromLinearPalette(frand());

            light.position = glm::vec3(frand() * size - 0.5 * size, frand() * size - 0.5 * size, frand() * 3.0);
            light.ambient = color * 0.0f;
            light.diffuse = color * 0.4f;
            light.specular = glm::vec3(0.5, 0.5, 0.5);
            light.attenuation0 = _attenuation0;
            light.attenuation1 = _attenuation1;
            light.attenuation2 = _attenuation2;

            _lights.push_back(light);
        }
    }

    void updateGUI() override
    {
        Application::updateGUI();

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
        if (ImGui::Begin("MIPT OpenGL Sample", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("FPS %.1f", ImGui::GetIO().Framerate);

            if (ImGui::CollapsingHeader("Light"))
            {
                ImGui::ColorEdit3("ambient", glm::value_ptr(_light.ambient));
                ImGui::ColorEdit3("diffuse", glm::value_ptr(_light.diffuse));
                ImGui::ColorEdit3("specular", glm::value_ptr(_light.specular));

                ImGui::SliderFloat("radius", &_lr, 0.1f, 10.0f);
                ImGui::SliderFloat("phi", &_phi, 0.0f, 2.0f * glm::pi<float>());
                ImGui::SliderFloat("theta", &_theta, 0.0f, glm::pi<float>());

                ImGui::SliderFloat("attenuation0", &_attenuation0, 1.0f, 10.0f);
                ImGui::SliderFloat("attenuation1", &_attenuation1, 0.0f, 1.0f);
                ImGui::SliderFloat("attenuation2", &_attenuation2, 0.0f, 1.0f);
            }

            ImGui::SliderInt("Mesh count", &_Ncurrent, 0, _Npositions);
            ImGui::SliderInt("Light count", &_Kcurrent, 0, _Klights);
        }
        ImGui::End();
    }

    void update()
    {
        Application::update();

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
    }

    void draw() override
    {
        drawToScreen(_camera);
    }

    void drawToScreen(const CameraInfo& camera)
    {
        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //-----------------------------------------------------

        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

        _prepassShader->use();
        _prepassShader->setMat4Uniform("viewMatrix", camera.viewMatrix);
        _prepassShader->setMat4Uniform("projectionMatrix", camera.projMatrix);

        drawScene(_prepassShader, _camera, glm::vec3(), false);

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        //-----------------------------------------------------

        _mainShader->use();
        _mainShader->setMat4Uniform("viewMatrix", camera.viewMatrix);
        _mainShader->setMat4Uniform("projectionMatrix", camera.projMatrix);

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0        
        glBindSampler(0, _sampler);
        _brickTex->bind();
        _mainShader->setIntUniform("diffuseTex", 0);

        glDepthFunc(GL_EQUAL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        //Параметры затухания сделаем общими для всех источников света
        _mainShader->setFloatUniform("light.a0", _attenuation0);
        _mainShader->setFloatUniform("light.a1", _attenuation1);
        _mainShader->setFloatUniform("light.a2", _attenuation2);

        glm::vec3 lightPosCamSpace = glm::vec3(camera.viewMatrix * glm::vec4(_light.position, 1.0));

        _mainShader->setVec3Uniform("light.pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
        _mainShader->setVec3Uniform("light.La", _light.ambient);
        _mainShader->setVec3Uniform("light.Ld", _light.diffuse);
        _mainShader->setVec3Uniform("light.Ls", _light.specular);

        drawScene(_mainShader, _camera, _light.position, true);

        for (int i = 0; i < _Kcurrent; i++)
        {
            glm::vec3 lightPosCamSpace = glm::vec3(camera.viewMatrix * glm::vec4(_lights[i].position, 1.0));

            _mainShader->setVec3Uniform("light.pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
            _mainShader->setVec3Uniform("light.La", _lights[i].ambient);
            _mainShader->setVec3Uniform("light.Ld", _lights[i].diffuse);
            _mainShader->setVec3Uniform("light.Ls", _lights[i].specular);

            drawScene(_mainShader, _camera, _lights[i].position, true);
        }

        glDisable(GL_BLEND);
        glDepthFunc(GL_LESS);

        //Рисуем маркеры для всех источников света		
        {
            _markerShader->use();

            _markerShader->setMat4Uniform("mvpMatrix", _camera.projMatrix * _camera.viewMatrix * glm::translate(glm::mat4(1.0f), _light.position));
            _markerShader->setVec4Uniform("color", glm::vec4(_light.diffuse, 1.0f));
            _marker->draw();

            for (unsigned int i = 0; i < _Kcurrent; i++)
            {
                _markerShader->setMat4Uniform("mvpMatrix", _camera.projMatrix * _camera.viewMatrix * glm::translate(glm::mat4(1.0f), _lights[i].position));
                _markerShader->setVec4Uniform("color", glm::vec4(_lights[i].diffuse, 1.0f));
                _marker->draw();
            }
        }

        //Отсоединяем сэмплер и шейдерную программу
        glBindSampler(0, 0);
        glUseProgram(0);
    }

    void drawScene(const ShaderProgramPtr& shader, const CameraInfo& camera, const glm::vec3& colorPos, bool checkDistance)
    {
        shader->setMat4Uniform("modelMatrix", _cube->modelMatrix());
        shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _cube->modelMatrix()))));

        _cube->draw();

        shader->setMat4Uniform("modelMatrix", _sphere->modelMatrix());
        shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _sphere->modelMatrix()))));

        _sphere->draw();

        shader->setMat4Uniform("modelMatrix", _ground->modelMatrix());
        shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _ground->modelMatrix()))));

        _ground->draw();

        shader->setMat4Uniform("modelMatrix", _bunny->modelMatrix());
        shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _bunny->modelMatrix()))));

        _bunny->draw();

        for (int i = 0; i < _Ncurrent; i++)
        {
            if (!checkDistance || (_positions[i] - colorPos).length() < 5.0f)
            {
                glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), _positions[i]);

                shader->setMat4Uniform("modelMatrix", modelMatrix);
                shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * modelMatrix))));

                _cube->draw();
            }
        }
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}