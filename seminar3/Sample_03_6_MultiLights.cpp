#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

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

struct MaterialInfo
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess = 128.0f;
};

constexpr unsigned int LightNum = 3;

/**
Несколько источников света
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _sphere;
    MeshPtr _bunny;

    MeshPtr _marker; //Маркер для источника света

    ShaderProgramPtr _shader;
    ShaderProgramPtr _markerShader;

    //Переменные для управления положением одного источника света
    float _lr = 3.0;
    float _phi = 0.0;
    float _theta = glm::pi<float>() * 0.25f;

    LightInfo _light[LightNum];
    MaterialInfo _material;

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

        _marker = makeSphere(0.1f);

        //=========================================================
        //Инициализация шейдеров

        _shader = std::make_shared<ShaderProgram>("shaders3/manyLights.vert", "shaders3/manyLights.frag");
        _markerShader = std::make_shared<ShaderProgram>("shaders/marker.vert", "shaders/marker.frag");

        //=========================================================
        //Инициализация значений переменных освщения
        _light[0].position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        _light[0].ambient = glm::vec3(0.2, 0.2, 0.2);
        _light[0].diffuse = glm::vec3(0.8, 0.8, 0.8);
        _light[0].specular = glm::vec3(1.0, 1.0, 1.0);

        _light[1].position = glm::vec3(0.0, 3.0, 3.0);
        _light[1].ambient = glm::vec3(0.2, 0.0, 0.0);
        _light[1].diffuse = glm::vec3(0.8, 0.0, 0.0);
        _light[1].specular = glm::vec3(1.0, 1.0, 1.0);

        _light[2].position = glm::vec3(0.0, -3.0, 3.0);
        _light[2].ambient = glm::vec3(0.0, 0.2, 0.0);
        _light[2].diffuse = glm::vec3(0.0, 0.8, 0.0);
        _light[2].specular = glm::vec3(1.0, 1.0, 1.0);

        //Инициализация материала кролика
        _material.ambient = glm::vec3(1.0, 1.0, 0.0);
        _material.diffuse = glm::vec3(1.0, 1.0, 0.0);
        _material.specular = glm::vec3(1.0, 1.0, 1.0);
        _material.shininess = 128.0f;
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
                ImGui::ColorEdit3("ambient", glm::value_ptr(_light[0].ambient));
                ImGui::ColorEdit3("diffuse", glm::value_ptr(_light[0].diffuse));
                ImGui::ColorEdit3("specular", glm::value_ptr(_light[0].specular));

                ImGui::SliderFloat("radius", &_lr, 0.1f, 10.0f);
                ImGui::SliderFloat("phi", &_phi, 0.0f, 2.0f * glm::pi<float>());
                ImGui::SliderFloat("theta", &_theta, 0.0f, glm::pi<float>());
            }

            if (ImGui::CollapsingHeader("Rabbit material"))
            {
                ImGui::ColorEdit3("mat ambient", glm::value_ptr(_material.ambient));
                ImGui::ColorEdit3("mat diffuse", glm::value_ptr(_material.diffuse));
                ImGui::ColorEdit3("mat specular", glm::value_ptr(_material.specular));
                ImGui::SliderFloat("shininess", &_material.shininess, 0.1f, 255.0f);
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

        _light[0].position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;

        for (unsigned int i = 0; i < LightNum; i++)
        {
            std::ostringstream str;
            str << "light[" << i << "]";

            glm::vec3 lightPosCamSpace = glm::vec3(_camera.viewMatrix * glm::vec4(_light[i].position, 1.0));

            _shader->setVec3Uniform(str.str() + ".pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
            _shader->setVec3Uniform(str.str() + ".La", _light[i].ambient);
            _shader->setVec3Uniform(str.str() + ".Ld", _light[i].diffuse);
            _shader->setVec3Uniform(str.str() + ".Ls", _light[i].specular);
        }

        //Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
        {
            _shader->setMat4Uniform("modelMatrix", _cube->modelMatrix());
            _shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _cube->modelMatrix()))));

            _shader->setVec3Uniform("material.Ka", glm::vec3(0.0, 1.0, 0.0));
            _shader->setVec3Uniform("material.Kd", glm::vec3(0.0, 1.0, 0.0));
            _shader->setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
            _shader->setFloatUniform("material.shininess", _material.shininess);

            _cube->draw();
        }

        {
            _shader->setMat4Uniform("modelMatrix", _sphere->modelMatrix());
            _shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _sphere->modelMatrix()))));

            _shader->setVec3Uniform("material.Ka", glm::vec3(1.0, 1.0, 1.0));
            _shader->setVec3Uniform("material.Kd", glm::vec3(1.0, 1.0, 1.0));
            _shader->setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
            _shader->setFloatUniform("material.shininess", _material.shininess);

            _sphere->draw();
        }

        {
            _shader->setMat4Uniform("modelMatrix", _bunny->modelMatrix());
            _shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _bunny->modelMatrix()))));

            _shader->setVec3Uniform("material.Ka", _material.ambient);
            _shader->setVec3Uniform("material.Kd", _material.diffuse);
            _shader->setVec3Uniform("material.Ks", _material.specular);
            _shader->setFloatUniform("material.shininess", _material.shininess);

            _bunny->draw();
        }

        //Рисуем маркеры для всех источников света		
        {
            _markerShader->use();

            for (unsigned int i = 0; i < LightNum; i++)
            {
                _markerShader->setMat4Uniform("mvpMatrix", _camera.projMatrix * _camera.viewMatrix * glm::translate(glm::mat4(1.0f), _light[i].position));
                _markerShader->setVec4Uniform("color", glm::vec4(_light[i].diffuse, 1.0f));
                _marker->draw();
            }
        }

        glUseProgram(0);
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}