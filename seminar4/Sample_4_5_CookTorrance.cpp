#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
Модель освещения Кука-Торренса
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _sphere;
    MeshPtr _bunny;

    MeshPtr _marker; //Меш - маркер для источника света

    //Идентификатор шейдерной программы
    ShaderProgram _shader;
    ShaderProgram _markerShader;

    float _lr;
    float _phi;
    float _theta;

    //переменные, которые содержат значения, которые будут записаны в uniform-переменные шейдеров
    glm::vec3 _lightAmbientColor;
    glm::vec3 _lightDiffuseColor;
    glm::vec3 _lightSpecularColor;

    glm::vec3 _rabbitAmbientColor;
    glm::vec3 _rabbitDiffuseColor;
    glm::vec3 _rabbitSpecularColor;
    float _roughnessValue;
    float _F0;

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

        _shader.createProgram("shaders4/CookTorrance.vert", "shaders4/CookTorrance.frag");
        _markerShader.createProgram("shaders4/marker.vert", "shaders4/marker.frag");

        //=========================================================
        //Инициализация значений переменных освщения
        _lr = 5.0;
        _phi = 0.0;
        _theta = glm::pi<float>() * 0.25f;

        _lightAmbientColor = glm::vec3(0.0, 0.0, 0.0);
        _lightDiffuseColor = glm::vec3(0.0, 0.0, 0.0);
        _lightSpecularColor = glm::vec3(1.0, 1.0, 1.0);

        //Инициализация материала кролика
        _rabbitAmbientColor = glm::vec3(1.0, 1.0, 0.0);
        _rabbitDiffuseColor = glm::vec3(1.0, 1.0, 0.0);
        _rabbitSpecularColor = glm::vec3(1.0, 1.0, 1.0);
        _roughnessValue = 0.3f;
        _F0 = 0.8f;
    }

    void initGUI() override
    {
        Application::initGUI();

        TwAddVarRW(_bar, "r", TW_TYPE_FLOAT, &_lr, "group=Light step=0.01 min=0.1 max=100.0");
        TwAddVarRW(_bar, "phi", TW_TYPE_FLOAT, &_phi, "group=Light step=0.01 min=0.0 max=6.28");
        TwAddVarRW(_bar, "theta", TW_TYPE_FLOAT, &_theta, "group=Light step=0.01 min=-1.57 max=1.57");
        TwAddVarRW(_bar, "La", TW_TYPE_COLOR3F, &_lightAmbientColor, "group=Light label='ambient'");
        TwAddVarRW(_bar, "Ld", TW_TYPE_COLOR3F, &_lightDiffuseColor, "group=Light label='diffuse'");
        TwAddVarRW(_bar, "Ls", TW_TYPE_COLOR3F, &_lightSpecularColor, "group=Light label='specular'");
        TwAddVarRW(_bar, "Ka", TW_TYPE_COLOR3F, &_rabbitAmbientColor, "group='Rabbit material' label='ambient'");
        TwAddVarRW(_bar, "Kd", TW_TYPE_COLOR3F, &_rabbitDiffuseColor, "group='Rabbit material' label='diffuse'");
        TwAddVarRW(_bar, "Ks", TW_TYPE_COLOR3F, &_rabbitSpecularColor, "group='Rabbit material' label='specular'");
        TwAddVarRW(_bar, "roughness", TW_TYPE_FLOAT, &_roughnessValue, "group='Rabbit material' step=0.01 min=0.1 max=1.0");
        TwAddVarRW(_bar, "F0", TW_TYPE_FLOAT, &_F0, "group='Rabbit material' step=0.01 min=0.1 max=1.0");
    }

    void draw() override
    {
        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 lightPos = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * _lr;

        //Подключаем шейдер		
        _shader.use();

        //Загружаем на видеокарту значения юниформ-переменных
        _shader.setMat4Uniform("viewMatrix", _camera.viewMatrix);
        _shader.setMat4Uniform("projectionMatrix", _camera.projMatrix);


        _shader.setVec3Uniform("light.pos", lightPos);
        _shader.setVec3Uniform("light.La", _lightAmbientColor);
        _shader.setVec3Uniform("light.Ld", _lightDiffuseColor);
        _shader.setVec3Uniform("light.Ls", _lightSpecularColor);

        //Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
        {
            _shader.setMat4Uniform("modelMatrix", _cube->modelMatrix());
            _shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _cube->modelMatrix()))));

            _shader.setVec3Uniform("material.Ka", glm::vec3(0.0, 1.0, 0.0));
            _shader.setVec3Uniform("material.Kd", glm::vec3(0.0, 1.0, 0.0));
            _shader.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
            _shader.setFloatUniform("material.roughnessValue", _roughnessValue);
            _shader.setFloatUniform("material.F0", _F0);

            _cube->draw();
        }

        {
            _shader.setMat4Uniform("modelMatrix", _sphere->modelMatrix());
            _shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _sphere->modelMatrix()))));

            _shader.setVec3Uniform("material.Ka", glm::vec3(1.0, 1.0, 1.0));
            _shader.setVec3Uniform("material.Kd", glm::vec3(1.0, 1.0, 1.0));
            _shader.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
            _shader.setFloatUniform("material.roughnessValue", _roughnessValue);
            _shader.setFloatUniform("material.F0", _F0);

            _sphere->draw();
        }

        {
            _shader.setMat4Uniform("modelMatrix", _bunny->modelMatrix());
            _shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _bunny->modelMatrix()))));

            _shader.setVec3Uniform("material.Ka", glm::vec3(_rabbitAmbientColor));
            _shader.setVec3Uniform("material.Kd", glm::vec3(_rabbitDiffuseColor));
            _shader.setVec3Uniform("material.Ks", glm::vec3(_rabbitSpecularColor));
            _shader.setFloatUniform("material.roughnessValue", _roughnessValue);
            _shader.setFloatUniform("material.F0", _F0);

            _bunny->draw();
        }

        //Рисуем маркер для источника света		
        {
            _markerShader.use();
            _markerShader.setMat4Uniform("mvpMatrix", _camera.projMatrix * _camera.viewMatrix * glm::translate(glm::mat4(1.0f), lightPos));
            _markerShader.setVec4Uniform("color", glm::vec4(_lightDiffuseColor, 1.0f));
            _marker->draw();
        }
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}