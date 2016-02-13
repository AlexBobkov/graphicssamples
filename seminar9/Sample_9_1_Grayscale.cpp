#include <Application.hpp>
#include <Framebuffer.hpp>
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

float frand()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

/**
Пример эффекта постобработки - вывод изображения в оттенках серого
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _sphere;
    MeshPtr _bunny;
    MeshPtr _ground;

    MeshPtr _quad;

    MeshPtr _marker; //Меш - маркер для источника света

    //Идентификатор шейдерной программы
    ShaderProgram _commonShader;
    ShaderProgram _markerShader;
    ShaderProgram _skyboxShader;
    ShaderProgram _quadDepthShader;
    ShaderProgram _quadColorShader;
    ShaderProgram _renderToShadowMapShader;
    ShaderProgram _renderToGBufferShader;
    ShaderProgram _renderDeferredShader;
    ShaderProgram _grayscaleShader;

    //Переменные для управления положением одного источника света
    float _lr;
    float _phi;
    float _theta;

    LightInfo _light;
    CameraInfo _lightCamera;

    TexturePtr _brickTex;

    GLuint _sampler;
    GLuint _cubeTexSampler;
    GLuint _depthSampler;

    bool _applyEffect;

    bool _showGBufferDebug;
    bool _showShadowDebug;
    bool _showDeferredDebug;

    Framebuffer _gbufferFB;
    GLuint _depthTexId;
    GLuint _normalsTexId;
    GLuint _diffuseTexId;

    Framebuffer _shadowFB;
    GLuint _shadowTexId;

    Framebuffer _deferredFB;
    GLuint _deferredTexId;

    //Старые размеры экрана
    int _oldWidth;
    int _oldHeight;

    void initFramebuffers()
    {
        _gbufferFB.create();
        _gbufferFB.bind();
        _gbufferFB.setSize(1024, 1024);

        _normalsTexId = _gbufferFB.addBuffer(GL_RGB16F, GL_COLOR_ATTACHMENT0);
        _diffuseTexId = _gbufferFB.addBuffer(GL_RGB8, GL_COLOR_ATTACHMENT1);
        _depthTexId = _gbufferFB.addBuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT);

        _gbufferFB.initDrawBuffers();

        if (!_gbufferFB.valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        _gbufferFB.unbind();

        //=========================================================

        _shadowFB.create();
        _shadowFB.bind();
        _shadowFB.setSize(1024, 1024);

        _shadowTexId = _shadowFB.addBuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT);

        _shadowFB.initDrawBuffers();

        if (!_shadowFB.valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        _shadowFB.unbind();

        //=========================================================

        _deferredFB.create();
        _deferredFB.bind();
        _deferredFB.setSize(1024, 1024);

        _deferredTexId = _deferredFB.addBuffer(GL_RGB8, GL_COLOR_ATTACHMENT0);

        _deferredFB.initDrawBuffers();

        if (!_deferredFB.valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        _deferredFB.unbind();
    }

    void makeScene() override
    {
        Application::makeScene();

        _applyEffect = true;
        _showGBufferDebug = false;
        _showShadowDebug = false;
        _showDeferredDebug = false;

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

        _quad = makeScreenAlignedQuad();

        //=========================================================
        //Инициализация шейдеров

        _commonShader.createProgram("shaders6/common.vert", "shaders6/common.frag");
        _markerShader.createProgram("shaders4/marker.vert", "shaders4/marker.frag");
        _skyboxShader.createProgram("shaders6/skybox.vert", "shaders6/skybox.frag");
        _quadDepthShader.createProgram("shaders7/quadDepth.vert", "shaders7/quadDepth.frag");
        _quadColorShader.createProgram("shaders7/quadColor.vert", "shaders7/quadColor.frag");
        _renderToShadowMapShader.createProgram("shaders8/toshadow.vert", "shaders8/toshadow.frag");
        _renderToGBufferShader.createProgram("shaders8/togbuffer.vert", "shaders8/togbuffer.frag");
        _renderDeferredShader.createProgram("shaders9/deferred.vert", "shaders9/deferred.frag");
        _grayscaleShader.createProgram("shaders9/quad.vert", "shaders9/grayscale.frag");

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

        glGenSamplers(1, &_cubeTexSampler);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        GLfloat border[] = { 1.0f, 0.0f, 0.0f, 1.0f };

        glGenSamplers(1, &_depthSampler);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glSamplerParameterfv(_depthSampler, GL_TEXTURE_BORDER_COLOR, border);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

        //=========================================================
        glfwGetFramebufferSize(_window, &_oldWidth, &_oldHeight);

        //=========================================================
        //Инициализация фреймбуфера для рендера теневой карты

        initFramebuffers();
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
        TwAddVarRO(_bar, "Grayscale", TW_TYPE_BOOLCPP, &_applyEffect, "");
    }

    virtual void handleKey(int key, int scancode, int action, int mods)
    {
        Application::handleKey(key, scancode, action, mods);

        if (action == GLFW_PRESS)
        {
            if (key == GLFW_KEY_1)
            {
                _applyEffect = !_applyEffect;
            }
            else if (key == GLFW_KEY_Z)
            {
                _showGBufferDebug = !_showGBufferDebug;
            }
            else if (key == GLFW_KEY_X)
            {
                _showShadowDebug = !_showShadowDebug;
            }
            else if (key == GLFW_KEY_C)
            {
                _showDeferredDebug = !_showDeferredDebug;
            }
        }
    }

    void update()
    {
        Application::update();

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        _lightCamera.viewMatrix = glm::lookAt(_light.position, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        _lightCamera.projMatrix = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 30.f);

        //Если размер окна изменился, то изменяем размеры фреймбуферов - перевыделяем память под текстуры

        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);
        if (width != _oldWidth || height != _oldHeight)
        {
            _gbufferFB.resize(width, height);
            _deferredFB.resize(width, height);

            _oldWidth = width;
            _oldHeight = height;
        }
    }

    void draw() override
    {
        //Рендерим геометрию сцены в G-буфер
        drawToGBuffer(_gbufferFB, _renderToGBufferShader, _camera);

        //Рендерим геометрию сцены в теневую карту с позиции источника света
        drawToShadowMap(_shadowFB, _renderToShadowMapShader, _lightCamera);

        //Выполняем отложенное освещение, заодно накладывает тени, а результат записываем в текстуру
        drawDeferred(_deferredFB, _renderDeferredShader, _camera, _lightCamera);

        //Выводим полученную текстуру на экран, попутно применяя эффект постобработки
        if (_applyEffect)
        {
            drawToScreen(_grayscaleShader);
        }
        else
        {
            drawToScreen(_quadColorShader);
        }

        //Отладочный рендер текстур
        drawDebug();
    }

    void drawToGBuffer(const Framebuffer& fb, const ShaderProgram& shader, const CameraInfo& camera)
    {
        fb.bind();

        glViewport(0, 0, fb.width(), fb.height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4Uniform("viewMatrix", camera.viewMatrix);
        shader.setMat4Uniform("projectionMatrix", camera.projMatrix);

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0
        _brickTex->bind();
        glBindSampler(0, _sampler);
        shader.setIntUniform("diffuseTex", 0);

        drawScene(shader, camera);

        glUseProgram(0); //Отключаем шейдер

        fb.unbind(); //Отключаем фреймбуфер
    }

    void drawToShadowMap(const Framebuffer& fb, const ShaderProgram& shader, const CameraInfo& lightCamera)
    {
        fb.bind();

        glViewport(0, 0, fb.width(), fb.height());
        glClear(GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4Uniform("viewMatrix", lightCamera.viewMatrix);
        shader.setMat4Uniform("projectionMatrix", lightCamera.projMatrix);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        drawScene(shader, lightCamera);

        glDisable(GL_CULL_FACE);

        glUseProgram(0);

        fb.unbind();
    }

    void drawDeferred(const Framebuffer& fb, const ShaderProgram& shader, const CameraInfo& camera, const CameraInfo& lightCamera)
    {
        fb.bind();

        glViewport(0, 0, fb.width(), fb.height());
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        shader.setMat4Uniform("viewMatrixInverse", glm::inverse(camera.viewMatrix));
        shader.setMat4Uniform("projMatrixInverse", glm::inverse(camera.projMatrix));

        glm::vec3 lightPosCamSpace = glm::vec3(camera.viewMatrix * glm::vec4(_light.position, 1.0));

        shader.setVec3Uniform("light.pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
        shader.setVec3Uniform("light.La", _light.ambient);
        shader.setVec3Uniform("light.Ld", _light.diffuse);
        shader.setVec3Uniform("light.Ls", _light.specular);

        shader.setMat4Uniform("lightViewMatrix", lightCamera.viewMatrix);
        shader.setMat4Uniform("lightProjectionMatrix", lightCamera.projMatrix);

        glm::mat4 projScaleBiasMatrix = glm::scale(glm::translate(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5)), glm::vec3(0.5, 0.5, 0.5));
        shader.setMat4Uniform("lightScaleBiasMatrix", projScaleBiasMatrix);

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0
        glBindTexture(GL_TEXTURE_2D, _normalsTexId);
        glBindSampler(0, _sampler);
        shader.setIntUniform("normalsTex", 0);

        glActiveTexture(GL_TEXTURE1);  //текстурный юнит 1
        glBindTexture(GL_TEXTURE_2D, _diffuseTexId);
        glBindSampler(1, _sampler);
        shader.setIntUniform("diffuseTex", 1);

        glActiveTexture(GL_TEXTURE2);  //текстурный юнит 2
        glBindTexture(GL_TEXTURE_2D, _depthTexId);
        glBindSampler(2, _sampler);
        shader.setIntUniform("depthTex", 2);

        glActiveTexture(GL_TEXTURE3);  //текстурный юнит 3
        glBindTexture(GL_TEXTURE_2D, _shadowTexId);
        glBindSampler(3, _depthSampler);
        shader.setIntUniform("shadowTex", 3);

        _quad->draw(); //main light

        glUseProgram(0);

        fb.unbind();
    }

    void drawToScreen(const ShaderProgram& shader)
    {
        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _deferredTexId);
        glBindSampler(0, _sampler);
        shader.setIntUniform("tex", 0);

        _quad->draw();

        //Отсоединяем сэмплер и шейдерную программу
        glBindSampler(0, 0);
        glUseProgram(0);
    }

    void drawScene(const ShaderProgram& shader, const CameraInfo& camera)
    {
        glFrontFace(GL_CW);

        shader.setMat4Uniform("modelMatrix", _cube->modelMatrix());
        shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _cube->modelMatrix()))));

        _cube->draw();

        shader.setMat4Uniform("modelMatrix", _sphere->modelMatrix());
        shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _sphere->modelMatrix()))));

        _sphere->draw();

        shader.setMat4Uniform("modelMatrix", _ground->modelMatrix());
        shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _ground->modelMatrix()))));

        _ground->draw();

        glFrontFace(GL_CCW);

        shader.setMat4Uniform("modelMatrix", _bunny->modelMatrix());
        shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _bunny->modelMatrix()))));

        _bunny->draw();
    }

    void drawDebug()
    {
        glClear(GL_DEPTH_BUFFER_BIT);

        int size = 500;

        if (_showGBufferDebug)
        {
            drawQuad(_quadDepthShader, _depthTexId, 0, 0, size, size);
            drawQuad(_quadColorShader, _normalsTexId, size, 0, size, size);
            drawQuad(_quadColorShader, _diffuseTexId, size * 2, 0, size, size);
        }
        else if (_showShadowDebug)
        {
            drawQuad(_quadDepthShader, _shadowTexId, 0, 0, size, size);
        }
        else if (_showDeferredDebug)
        {
            drawQuad(_quadColorShader, _deferredTexId, 0, 0, size, size);
        }

        glBindSampler(0, 0);
        glUseProgram(0);
    }

    void drawQuad(const ShaderProgram& shader, GLuint& texId, GLint x, GLint y, GLint width, GLint height)
    {
        glViewport(x, y, width, height);

        shader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texId);
        glBindSampler(0, _sampler);
        shader.setIntUniform("tex", 0);

        _quad->draw();
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}