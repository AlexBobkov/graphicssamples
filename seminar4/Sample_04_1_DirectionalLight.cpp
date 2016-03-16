#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
Направленный источник света
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _sphere;
    MeshPtr _bunny;

    ShaderProgramPtr _shader;

    //Координаты источника света
    float _phi;
    float _theta;

    //Параметры источника света
    glm::vec3 _lightAmbientColor;
    glm::vec3 _lightDiffuseColor;

    //Параметры материала
    glm::vec3 _bunnyAmbientColor;
    glm::vec3 _bunnyDiffuseColor;

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

        //=========================================================
        //Инициализация шейдеров

        _shader = std::make_shared<ShaderProgram>();
        _shader->createProgram("shaders4/diffuseDirectionalLight.vert", "shaders4/diffuseDirectionalLight.frag");

        //=========================================================
        //Инициализация значений переменных освщения
        _phi = 0.0;
        _theta = glm::pi<float>() * 0.25f;

        _lightAmbientColor = glm::vec3(0.2, 0.2, 0.2);
        _lightDiffuseColor = glm::vec3(0.8, 0.8, 0.8);

        //Инициализация материала кролика
        _bunnyAmbientColor = glm::vec3(1.0, 1.0, 0.0);
        _bunnyDiffuseColor = glm::vec3(1.0, 1.0, 0.0);
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
                ImGui::ColorEdit3("ambient", glm::value_ptr(_lightAmbientColor));
                ImGui::ColorEdit3("diffuse", glm::value_ptr(_lightDiffuseColor));

                ImGui::SliderFloat("phi", &_phi, 0.0f, 2.0f * glm::pi<float>());
                ImGui::SliderFloat("theta", &_theta, 0.0f, glm::pi<float>());
            }

            if (ImGui::CollapsingHeader("Rabbit material"))
            {
                ImGui::ColorEdit3("mat ambient", glm::value_ptr(_bunnyAmbientColor));
                ImGui::ColorEdit3("mat diffuse", glm::value_ptr(_bunnyDiffuseColor));
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

        //Подключаем шейдер		
        _shader->use();

        //Загружаем на видеокарту значения юниформ-переменных
        _shader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        _shader->setMat4Uniform("projectionMatrix", _camera.projMatrix);

        glm::vec3 lightDir = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta));
        _shader->setVec3Uniform("light.dir", lightDir);
        _shader->setVec3Uniform("light.La", _lightAmbientColor);
        _shader->setVec3Uniform("light.Ld", _lightDiffuseColor);

        //Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
        {
            _shader->setMat4Uniform("modelMatrix", _cube->modelMatrix());
            _shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _cube->modelMatrix()))));

            _shader->setVec3Uniform("material.Ka", glm::vec3(0.0, 1.0, 0.0));
            _shader->setVec3Uniform("material.Kd", glm::vec3(0.0, 1.0, 0.0));

            _cube->draw();
        }

        {
            _shader->setMat4Uniform("modelMatrix", _sphere->modelMatrix());
            _shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _sphere->modelMatrix()))));

            _shader->setVec3Uniform("material.Ka", glm::vec3(1.0, 1.0, 1.0));
            _shader->setVec3Uniform("material.Kd", glm::vec3(1.0, 1.0, 1.0));

            _sphere->draw();
        }

        {
            _shader->setMat4Uniform("modelMatrix", _bunny->modelMatrix());
            _shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _bunny->modelMatrix()))));

            _shader->setVec3Uniform("material.Ka", glm::vec3(_bunnyAmbientColor));
            _shader->setVec3Uniform("material.Kd", glm::vec3(_bunnyDiffuseColor));

            _bunny->draw();
        }
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}