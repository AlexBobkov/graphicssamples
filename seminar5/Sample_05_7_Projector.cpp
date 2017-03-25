#include <Application.hpp>
#include <LightInfo.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>
#include <Texture.hpp>

#include <iostream>
#include <sstream>
#include <vector>

/**
Пример с проективной текстурой
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _sphere;
    MeshPtr _bunny;
    MeshPtr _ground;

    MeshPtr _marker; //Меш - маркер для источника света

    //Идентификатор шейдерной программы
    ShaderProgramPtr _markerShader;
    ShaderProgramPtr _projectorShader;

    //Переменные для управления положением одного источника света
    float _lr = 10.0f;
    float _phi = 2.65f;
    float _theta = 0.48f;

    LightInfo _light;

    TexturePtr _worldTex;
    TexturePtr _brickTex;

    GLuint _sampler;
    GLuint _projSampler;

    float _projR = 4.0f;
    float _projPhi = 0.0f;
    float _projTheta = 1.0f;

    CameraInfo _projCamera; //Для управления проектором можно использовать те же настройки, что и для виртуальной камеры

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

        _ground = makeGroundPlane(5.0f, 2.0f);

        _marker = makeSphere(0.1f);

        //=========================================================
        //Инициализация шейдеров

        _projectorShader = std::make_shared<ShaderProgram>("shaders5/projector.vert", "shaders5/projector.frag");
        _markerShader = std::make_shared<ShaderProgram>("shaders/marker.vert", "shaders/marker.frag");

        //=========================================================
        //Инициализация значений переменных освщения
        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * _lr;
        _light.ambient = glm::vec3(0.2, 0.2, 0.2);
        _light.diffuse = glm::vec3(0.8, 0.8, 0.8);
        _light.specular = glm::vec3(1.0, 1.0, 1.0);

        //=========================================================
        //Загрузка и создание текстур
        _worldTex = loadTexture("images/earth_global.jpg");
        _brickTex = loadTexture("images/brick.jpg");

        //=========================================================
        //Инициализация сэмплера, объекта, который хранит параметры чтения из текстуры
        glGenSamplers(1, &_sampler);
        glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenSamplers(1, &_projSampler);
        glSamplerParameteri(_projSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_projSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_projSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glSamplerParameteri(_projSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        //=========================================================
        //Инициализация проектора
        glm::vec3 projPos = glm::vec3(glm::cos(_projPhi) * glm::cos(_projTheta), glm::sin(_projPhi) * glm::cos(_projTheta), glm::sin(_projTheta)) * _projR;

        _projCamera.viewMatrix = glm::lookAt(projPos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        _projCamera.projMatrix = glm::perspective(glm::radians(25.0f), 1.0f, 0.1f, 100.f);
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
            }

            if (ImGui::CollapsingHeader("Projector"))
            {
                ImGui::SliderFloat("projector phi", &_projPhi, 0.0f, 2.0f * glm::pi<float>());
                ImGui::SliderFloat("projector theta", &_projTheta, 0.0f, glm::pi<float>());
            }
        }
        ImGui::End();
    }

    void draw() override
    {
        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //====== РИСУЕМ ОСНОВНЫЕ ОБЪЕКТЫ СЦЕНЫ ======
        _projectorShader->use();

        //Загружаем на видеокарту значения юниформ-переменных
        _projectorShader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        _projectorShader->setMat4Uniform("projectionMatrix", _camera.projMatrix);

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * _lr;
        glm::vec3 lightPosCamSpace = glm::vec3(_camera.viewMatrix * glm::vec4(_light.position, 1.0));

        _projectorShader->setVec3Uniform("light.pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
        _projectorShader->setVec3Uniform("light.La", _light.ambient);
        _projectorShader->setVec3Uniform("light.Ld", _light.diffuse);
        _projectorShader->setVec3Uniform("light.Ls", _light.specular);

        {
            glm::vec3 projPos = glm::vec3(glm::cos(_projPhi) * glm::cos(_projTheta), glm::sin(_projPhi) * glm::cos(_projTheta), glm::sin(_projTheta)) * _projR;

            _projCamera.viewMatrix = glm::lookAt(projPos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            _projCamera.projMatrix = glm::perspective(glm::radians(25.0f), 1.0f, 0.1f, 100.f);

            _projectorShader->setMat4Uniform("projViewMatrix", _projCamera.viewMatrix);
            _projectorShader->setMat4Uniform("projProjectionMatrix", _projCamera.projMatrix);

            glm::mat4 projScaleBiasMatrix = glm::scale(glm::translate(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5)), glm::vec3(0.5, 0.5, 0.5));
            _projectorShader->setMat4Uniform("projScaleBiasMatrix", projScaleBiasMatrix);
        }

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0        
        glBindSampler(0, _sampler);
        _brickTex->bind();
        _projectorShader->setIntUniform("diffuseTex", 0);

        glActiveTexture(GL_TEXTURE1);  //текстурный юнит 1        
        glBindSampler(1, _projSampler);
        _worldTex->bind();
        _projectorShader->setIntUniform("projTex", 1);

        //Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
        {
            _projectorShader->setMat4Uniform("modelMatrix", _cube->modelMatrix());
            _projectorShader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _cube->modelMatrix()))));

            _cube->draw();
        }

        {
            _projectorShader->setMat4Uniform("modelMatrix", _sphere->modelMatrix());
            _projectorShader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _sphere->modelMatrix()))));

            _sphere->draw();
        }

        {
            _projectorShader->setMat4Uniform("modelMatrix", _bunny->modelMatrix());
            _projectorShader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _bunny->modelMatrix()))));

            _bunny->draw();
        }

        {
            _projectorShader->setMat4Uniform("modelMatrix", _ground->modelMatrix());
            _projectorShader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _ground->modelMatrix()))));

            _ground->draw();
        }

        //Рисуем маркеры для всех источников света		
        {
            _markerShader->use();

            _markerShader->setMat4Uniform("mvpMatrix", _camera.projMatrix * _camera.viewMatrix * glm::translate(glm::mat4(1.0f), _light.position));
            _markerShader->setVec4Uniform("color", glm::vec4(_light.diffuse, 1.0f));
            _marker->draw();
        }

        //Рисуем маркер для проектора
        {
            _markerShader->use();

            glm::vec3 projPos = glm::vec3(glm::cos(_projPhi) * glm::cos(_projTheta), glm::sin(_projPhi) * glm::cos(_projTheta), glm::sin(_projTheta)) * _projR;

            _markerShader->setMat4Uniform("mvpMatrix", _camera.projMatrix * _camera.viewMatrix * glm::translate(glm::mat4(1.0f), projPos));
            _markerShader->setVec4Uniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            _marker->draw();
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