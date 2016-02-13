#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>
#include <Texture.hpp>

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

/**
Пример с буфером трафарета
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _sphere;
    MeshPtr _bunny;

    MeshPtr _marker; //Меш - маркер для источника света

    //Идентификатор шейдерной программы
    ShaderProgram _commonShader;
    ShaderProgram _markerShader;
    ShaderProgram _skyboxShader;

    //Переменные для управления положением одного источника света
    float _lr;
    float _phi;
    float _theta;

    LightInfo _light;

    GLuint _worldTexId;
    GLuint _brickTexId;
    GLuint _grassTexId;
    GLuint _chessTexId;
    GLuint _myTexId;
    GLuint _cubeTexId;

    GLuint _sampler;
    GLuint _cubeTexSampler;

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

        _commonShader.createProgram("shaders6/common.vert", "shaders6/common.frag");
        _markerShader.createProgram("shaders4/marker.vert", "shaders4/marker.frag");
        _skyboxShader.createProgram("shaders6/skybox.vert", "shaders6/skybox.frag");

        //=========================================================
        //Инициализация значений переменных освщения
        _lr = 10.0;
        _phi = 0.0f;
        _theta = 0.48f;

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        _light.ambient = glm::vec3(0.2, 0.2, 0.2);
        _light.diffuse = glm::vec3(0.8, 0.8, 0.8);
        _light.specular = glm::vec3(1.0, 1.0, 1.0);

        //=========================================================
        //Загрузка и создание текстур
        _worldTexId = Texture::loadTexture("images/earth_global.jpg");
        _brickTexId = Texture::loadTexture("images/brick.jpg");
        _grassTexId = Texture::loadTexture("images/grass.jpg");
        _chessTexId = Texture::loadTextureWithMipmaps("images/chess.dds");
        _myTexId = Texture::makeProceduralTexture();
        _cubeTexId = Texture::loadCubeTexture("images/cube");

        //=========================================================
        //Инициализация сэмплера, объекта, который хранит параметры чтения из текстуры
        glGenSamplers(1, &_sampler);
        glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenSamplers(1, &_cubeTexSampler);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    void initGUI() override
    {
        Application::initGUI();

        TwAddVarRW(_bar, "r", TW_TYPE_FLOAT, &_lr, "group=Light step=0.01 min=0.1 max=100.0");
        TwAddVarRW(_bar, "phi", TW_TYPE_FLOAT, &_phi, "group=Light step=0.01 min=0.0 max=6.28");
        TwAddVarRW(_bar, "theta", TW_TYPE_FLOAT, &_theta, "group=Light step=0.01 min=-1.57 max=1.57");
        TwAddVarRW(_bar, "La", TW_TYPE_COLOR3F, &_light.ambient, "group=Light label='ambient'");
        TwAddVarRW(_bar, "Ld", TW_TYPE_COLOR3F, &_light.diffuse, "group=Light label='diffuse'");
        TwAddVarRW(_bar, "Ls", TW_TYPE_COLOR3F, &_light.specular, "group=Light label='specular'");
    }

    void draw() override
    {
        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        //====== РИСУЕМ ОСНОВНЫЕ ОБЪЕКТЫ СЦЕНЫ ======
        _commonShader.use();

        //Загружаем на видеокарту значения юниформ-переменных
        _commonShader.setMat4Uniform("viewMatrix", _camera.viewMatrix);
        _commonShader.setMat4Uniform("projectionMatrix", _camera.projMatrix);

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        glm::vec3 lightPosCamSpace = glm::vec3(_camera.viewMatrix * glm::vec4(_light.position, 1.0));

        _commonShader.setVec3Uniform("light.pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
        _commonShader.setVec3Uniform("light.La", _light.ambient);
        _commonShader.setVec3Uniform("light.Ld", _light.diffuse);
        _commonShader.setVec3Uniform("light.Ls", _light.specular);

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0
        glBindTexture(GL_TEXTURE_2D, _grassTexId);
        glBindSampler(0, _sampler);
        _commonShader.setIntUniform("diffuseTex", 0);

        //Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
        {
            _commonShader.setMat4Uniform("modelMatrix", _cube->modelMatrix());
            _commonShader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _cube->modelMatrix()))));

            _cube->draw();
        }

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0
        glBindTexture(GL_TEXTURE_2D, _brickTexId);
        glBindSampler(0, _sampler);
        _commonShader.setIntUniform("diffuseTex", 0);

        glEnable(GL_STENCIL_TEST);

        glStencilFunc(GL_ALWAYS, 1, 1);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

        {
            _commonShader.setMat4Uniform("modelMatrix", _sphere->modelMatrix());
            _commonShader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _sphere->modelMatrix()))));

            _sphere->draw();
        }

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        glStencilFunc(GL_NOTEQUAL, 1, 1);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

        {
            _commonShader.setMat4Uniform("modelMatrix", _bunny->modelMatrix());
            _commonShader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _bunny->modelMatrix()))));

            _bunny->draw();
        }

        glDisable(GL_STENCIL_TEST);

        //Рисуем маркеры для всех источников света		
        {
            _markerShader.use();

            _markerShader.setMat4Uniform("mvpMatrix", _camera.projMatrix * _camera.viewMatrix * glm::translate(glm::mat4(1.0f), _light.position));
            _markerShader.setVec4Uniform("color", glm::vec4(_light.diffuse, 1.0f));
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