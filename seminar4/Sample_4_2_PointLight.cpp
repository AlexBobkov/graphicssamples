#include "Application.h"

#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
Точечный источник света
*/
class SampleApplication : public Application
{
public:
    Mesh cube;
    Mesh sphere;
    Mesh bunny;

    Mesh marker; //Меш - маркер для источника света

    //Идентификатор шейдерной программы
    ShaderProgram _shader;
    ShaderProgram _markerShader;

    float _lr;
    float _phi;
    float _theta;

    //переменные, которые содержат значения, которые будут записаны в uniform-переменные шейдеров
    glm::vec3 _lightAmbientColor;
    glm::vec3 _lightDiffuseColor;

    glm::vec3 _rabbitAmbientColor;
    glm::vec3 _rabbitDiffuseColor;

    virtual void makeScene()
    {
        Application::makeScene();

        //=========================================================
        //Создание и загрузка мешей		

        cube.makeCube(0.5);
        cube.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f));

        sphere.makeSphere(0.5);
        sphere.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f));

        bunny.loadFromFile("models/bunny.obj");
        bunny.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        marker.makeSphere(0.1);

        //=========================================================
        //Инициализация шейдеров

        _shader.createProgram("shaders4/diffusePointLight.vert", "shaders4/diffusePointLight.frag");
        _markerShader.createProgram("shaders4/marker.vert", "shaders4/marker.frag");

        //=========================================================
        //Инициализация значений переменных освщения
        _lr = 5.0;
        _phi = 0.0;
        _theta = glm::pi<float>() * 0.25f;

        _lightAmbientColor = glm::vec3(0.2, 0.2, 0.2);
        _lightDiffuseColor = glm::vec3(0.8, 0.8, 0.8);

        //Инициализация материала кролика
        _rabbitAmbientColor = glm::vec3(1.0, 1.0, 0.0);
        _rabbitDiffuseColor = glm::vec3(1.0, 1.0, 0.0);
    }

    virtual void initGUI()
    {
        Application::initGUI();

        TwAddVarRW(_bar, "r", TW_TYPE_FLOAT, &_lr, "group=Light step=0.01 min=0.1 max=100.0");
        TwAddVarRW(_bar, "phi", TW_TYPE_FLOAT, &_phi, "group=Light step=0.01 min=0.0 max=6.28");
        TwAddVarRW(_bar, "theta", TW_TYPE_FLOAT, &_theta, "group=Light step=0.01 min=-1.57 max=1.57");
        TwAddVarRW(_bar, "La", TW_TYPE_COLOR3F, &_lightAmbientColor, "group=Light label='ambient'");
        TwAddVarRW(_bar, "Ld", TW_TYPE_COLOR3F, &_lightDiffuseColor, "group=Light label='diffuse'");
        TwAddVarRW(_bar, "Ka", TW_TYPE_COLOR3F, &_rabbitAmbientColor, "group='Rabbit material' label='ambient'");
        TwAddVarRW(_bar, "Kd", TW_TYPE_COLOR3F, &_rabbitDiffuseColor, "group='Rabbit material' label='diffuse'");
    }

    virtual void draw()
    {
        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Подключаем шейдер		
        _shader.use();

        //Загружаем на видеокарту значения юниформ-переменных
        _shader.setMat4Uniform("viewMatrix", _viewMatrix);
        _shader.setMat4Uniform("projectionMatrix", _projMatrix);

        glm::vec3 lightPos = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * _lr;
        _shader.setVec3Uniform("light.pos", lightPos);
        _shader.setVec3Uniform("light.La", _lightAmbientColor);
        _shader.setVec3Uniform("light.Ld", _lightDiffuseColor);

        //Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
        {
            _shader.setMat4Uniform("modelMatrix", cube.modelMatrix());
            _shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * cube.modelMatrix()))));

            _shader.setVec3Uniform("material.Ka", glm::vec3(0.0, 1.0, 0.0));
            _shader.setVec3Uniform("material.Kd", glm::vec3(0.0, 1.0, 0.0));

            cube.draw();
        }

        {
            _shader.setMat4Uniform("modelMatrix", sphere.modelMatrix());
            _shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * sphere.modelMatrix()))));

            _shader.setVec3Uniform("material.Ka", glm::vec3(1.0, 1.0, 1.0));
            _shader.setVec3Uniform("material.Kd", glm::vec3(1.0, 1.0, 1.0));

            sphere.draw();
        }

        {
            _shader.setMat4Uniform("modelMatrix", bunny.modelMatrix());
            _shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * bunny.modelMatrix()))));

            _shader.setVec3Uniform("material.Ka", glm::vec3(_rabbitAmbientColor));
            _shader.setVec3Uniform("material.Kd", glm::vec3(_rabbitDiffuseColor));

            bunny.draw();
        }

        //Рисуем маркер для источника света		
        {
            _markerShader.use();
            _markerShader.setMat4Uniform("mvpMatrix", _projMatrix * _viewMatrix * glm::translate(glm::mat4(1.0f), lightPos));
            _markerShader.setVec4Uniform("color", glm::vec4(_lightDiffuseColor, 1.0f));
            marker.draw();
        }
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}