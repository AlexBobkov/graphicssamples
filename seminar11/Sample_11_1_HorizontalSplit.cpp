#include <Application.hpp>
#include <LightInfo.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>
#include <Texture.hpp>

#include <iostream>
#include <sstream>
#include <vector>
#include <deque>

/**
Пример построения горизонтальной стереопары
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _sphere;
    MeshPtr _bunny;
    MeshPtr _ground;

    ShaderProgramPtr _commonShader;

    //Переменные для управления положением одного источника света
    float _lr;
    float _phi;
    float _theta;

    LightInfo _light;
    CameraInfo _lightCamera;

    TexturePtr _brickTex;

    GLuint _sampler;

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

        //=========================================================
        //Инициализация шейдеров

        _commonShader = std::make_shared<ShaderProgram>();
        _commonShader->createProgram("shaders/common.vert", "shaders/common.frag");

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
        _brickTex = loadTexture("images/brick.jpg");

        //=========================================================
        //Инициализация сэмплера, объекта, который хранит параметры чтения из текстуры
        glGenSamplers(1, &_sampler);
        glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }    

    void update()
    {
        Application::update();

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        _lightCamera.viewMatrix = glm::lookAt(_light.position, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        _lightCamera.projMatrix = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 30.f);
    }

    void draw() override
    {
        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        CameraInfo leftCamera;
        CameraInfo rightCamera;

        float IOD = 0.06f;
        float halfIOD = IOD * 0.5f;
        float SD = 0.4f;

        glm::mat4 leftShiftMat = glm::mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            halfIOD / SD, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        glm::mat4 rightShiftMat = glm::mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            -halfIOD / SD, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        leftCamera.projMatrix = _camera.projMatrix * glm::scale(leftShiftMat, glm::vec3(2.0f, 1.0f, 1.0f));
        rightCamera.projMatrix = _camera.projMatrix * glm::scale(rightShiftMat, glm::vec3(2.0f, 1.0f, 1.0f));

        leftCamera.viewMatrix = glm::translate(_camera.viewMatrix, glm::vec3(-halfIOD, 0, 0));
        rightCamera.viewMatrix = glm::translate(_camera.viewMatrix, glm::vec3(halfIOD, 0, 0));


        glClearColor(199.0f / 255, 221.0f / 255, 235.0f / 255, 1); //blue color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, width / 2, height);
        drawScene(_commonShader, leftCamera); //left

        glViewport(width / 2, 0, width / 2, height);
        drawScene(_commonShader, rightCamera); //right

        //Отсоединяем сэмплер и шейдерную программу
        glBindSampler(0, 0);
        glUseProgram(0);
    }

    void drawScene(const ShaderProgramPtr& shader, const CameraInfo& camera)
    {
        shader->use();

        //Загружаем на видеокарту значения юниформ-переменных
        shader->setMat4Uniform("viewMatrix", camera.viewMatrix);
        shader->setMat4Uniform("projectionMatrix", camera.projMatrix);

        glm::vec3 lightPosCamSpace = glm::vec3(camera.viewMatrix * glm::vec4(_light.position, 1.0));
        shader->setVec3Uniform("light.pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
        shader->setVec3Uniform("light.La", _light.ambient);
        shader->setVec3Uniform("light.Ld", _light.diffuse);
        shader->setVec3Uniform("light.Ls", _light.specular);

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0        
        glBindSampler(0, _sampler);
        _brickTex->bind();
        shader->setIntUniform("diffuseTex", 0);

        //Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
        {
            shader->setMat4Uniform("modelMatrix", _cube->modelMatrix());
            shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _cube->modelMatrix()))));

            _cube->draw();
        }

        {
            shader->setMat4Uniform("modelMatrix", _sphere->modelMatrix());
            shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _sphere->modelMatrix()))));

            _sphere->draw();
        }

        {
            shader->setMat4Uniform("modelMatrix", _bunny->modelMatrix());
            shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _bunny->modelMatrix()))));

            _bunny->draw();
        }

        {
            shader->setMat4Uniform("modelMatrix", _ground->modelMatrix());
            shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _ground->modelMatrix()))));

            _ground->draw();
        }
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}