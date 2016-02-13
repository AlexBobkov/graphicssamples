#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
Точечный источник света. 2 варианта шейдеров: с повершиным и пофрагментым освещением
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _sphere;
    MeshPtr _bunny;

    MeshPtr _marker; //Меш - маркер для источника света

    //Идентификатор шейдерной программы
    ShaderProgram _shaderPerVertex;
    ShaderProgram _shaderPerFragment;
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
    float _rabbitShininess;

    void makeScene() override
    {
        Application::makeScene();

        //=========================================================
        //Создание и загрузка мешей		

        _cube = makeCube(0.5f);
        _cube->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f)));

        _sphere = makeSphere(0.5f, 40);
        _sphere->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f)));

        _bunny = loadFromFile("models/bunny.obj");
        _bunny->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

        _marker = makeSphere(0.1f);

        //=========================================================
        //Инициализация шейдеров

        _shaderPerVertex.createProgram("shaders4/specularPointLightPerVertex.vert", "shaders4/specularPointLightPerVertex.frag");
        _shaderPerFragment.createProgram("shaders4/specularPointLightPerFragment.vert", "shaders4/specularPointLightPerFragment.frag");
        _markerShader.createProgram("shaders4/marker.vert", "shaders4/marker.frag");

        //=========================================================
        //Инициализация значений переменных освщения
        _lr = 5.0;
        _phi = 0.0;
        _theta = glm::pi<float>() * 0.25f;

        _lightAmbientColor = glm::vec3(0.2, 0.2, 0.2);
        _lightDiffuseColor = glm::vec3(0.8, 0.8, 0.8);
        _lightSpecularColor = glm::vec3(1.0, 1.0, 1.0);

        //Инициализация материала кролика
        _rabbitAmbientColor = glm::vec3(1.0, 1.0, 0.0);
        _rabbitDiffuseColor = glm::vec3(1.0, 1.0, 0.0);
        _rabbitSpecularColor = glm::vec3(1.0, 1.0, 1.0);
        _rabbitShininess = 128.0f;
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
        TwAddVarRW(_bar, "shininess", TW_TYPE_FLOAT, &_rabbitShininess, "group='Rabbit material' min=0.1 max=255.0");
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

        //Сначала применяем шейдер с повершинным освещением
        {
            //Подключаем шейдер		
            _shaderPerVertex.use();

            //Загружаем на видеокарту значения юниформ-переменных
            _shaderPerVertex.setMat4Uniform("viewMatrix", _camera.viewMatrix);
            _shaderPerVertex.setMat4Uniform("projectionMatrix", _camera.projMatrix);


            _shaderPerVertex.setVec3Uniform("light.pos", lightPos);
            _shaderPerVertex.setVec3Uniform("light.La", _lightAmbientColor);
            _shaderPerVertex.setVec3Uniform("light.Ld", _lightDiffuseColor);
            _shaderPerVertex.setVec3Uniform("light.Ls", _lightSpecularColor);

            //Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
            {
                _shaderPerVertex.setMat4Uniform("modelMatrix", _cube->modelMatrix());
                _shaderPerVertex.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _cube->modelMatrix()))));

                _shaderPerVertex.setVec3Uniform("material.Ka", glm::vec3(0.0, 1.0, 0.0));
                _shaderPerVertex.setVec3Uniform("material.Kd", glm::vec3(0.0, 1.0, 0.0));
                _shaderPerVertex.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
                _shaderPerVertex.setFloatUniform("material.shininess", _rabbitShininess);

                _cube->draw();
            }

            {
                _shaderPerVertex.setMat4Uniform("modelMatrix", _sphere->modelMatrix());
                _shaderPerVertex.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _sphere->modelMatrix()))));

                _shaderPerVertex.setVec3Uniform("material.Ka", glm::vec3(1.0, 1.0, 1.0));
                _shaderPerVertex.setVec3Uniform("material.Kd", glm::vec3(1.0, 1.0, 1.0));
                _shaderPerVertex.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
                _shaderPerVertex.setFloatUniform("material.shininess", _rabbitShininess);

                _sphere->draw();
            }

            {
                _shaderPerVertex.setMat4Uniform("modelMatrix", _bunny->modelMatrix());
                _shaderPerVertex.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _bunny->modelMatrix()))));

                _shaderPerVertex.setVec3Uniform("material.Ka", glm::vec3(_rabbitAmbientColor));
                _shaderPerVertex.setVec3Uniform("material.Kd", glm::vec3(_rabbitDiffuseColor));
                _shaderPerVertex.setVec3Uniform("material.Ks", glm::vec3(_rabbitSpecularColor));
                _shaderPerVertex.setFloatUniform("material.shininess", _rabbitShininess);

                _bunny->draw();
            }
        }

        //Сначала применяем шейдер с пофрагментным освещением
        {
            //Подключаем шейдер		
            _shaderPerFragment.use();

            //Загружаем на видеокарту значения юниформ-переменных
            _shaderPerFragment.setMat4Uniform("viewMatrix", _camera.viewMatrix);
            _shaderPerFragment.setMat4Uniform("projectionMatrix", _camera.projMatrix);


            _shaderPerFragment.setVec3Uniform("light.pos", lightPos);
            _shaderPerFragment.setVec3Uniform("light.La", _lightAmbientColor);
            _shaderPerFragment.setVec3Uniform("light.Ld", _lightDiffuseColor);
            _shaderPerFragment.setVec3Uniform("light.Ls", _lightSpecularColor);

            //Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
            {
                _shaderPerFragment.setMat4Uniform("modelMatrix", glm::translate(_cube->modelMatrix(), glm::vec3(1.5f, 0.0f, 0.0f)));
                _shaderPerFragment.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _cube->modelMatrix()))));

                _shaderPerFragment.setVec3Uniform("material.Ka", glm::vec3(0.0, 1.0, 0.0));
                _shaderPerFragment.setVec3Uniform("material.Kd", glm::vec3(0.0, 1.0, 0.0));
                _shaderPerFragment.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
                _shaderPerFragment.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
                _shaderPerFragment.setFloatUniform("material.shininess", _rabbitShininess);

                _cube->draw();
            }

            {
                _shaderPerFragment.setMat4Uniform("modelMatrix", glm::translate(_sphere->modelMatrix(), glm::vec3(1.5f, 0.0f, 0.0f)));
                _shaderPerFragment.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _sphere->modelMatrix()))));

                _shaderPerFragment.setVec3Uniform("material.Ka", glm::vec3(1.0, 1.0, 1.0));
                _shaderPerFragment.setVec3Uniform("material.Kd", glm::vec3(1.0, 1.0, 1.0));
                _shaderPerFragment.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
                _shaderPerFragment.setFloatUniform("material.shininess", _rabbitShininess);

                _sphere->draw();
            }

            {
                _shaderPerFragment.setMat4Uniform("modelMatrix", glm::translate(_bunny->modelMatrix(), glm::vec3(1.5f, 0.0f, 0.0f)));
                _shaderPerFragment.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _bunny->modelMatrix()))));

                _shaderPerFragment.setVec3Uniform("material.Ka", glm::vec3(_rabbitAmbientColor));
                _shaderPerFragment.setVec3Uniform("material.Kd", glm::vec3(_rabbitDiffuseColor));
                _shaderPerFragment.setVec3Uniform("material.Ks", glm::vec3(_rabbitSpecularColor));
                _shaderPerFragment.setFloatUniform("material.shininess", _rabbitShininess);

                _bunny->draw();
            }
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