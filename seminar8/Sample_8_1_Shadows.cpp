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
Пример с тенями
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
    ShaderProgram _quadShader;
    ShaderProgram _renderToShadowMapShader;
    ShaderProgram _commonWithShadowsShader;
    ShaderProgram _commonWithShadowsShaderVar2;

    //Переменные для управления положением одного источника света
    float _lr;
    float _phi;
    float _theta;

    LightInfo _light;
    CameraInfo _lightCamera;

    GLuint _worldTexId;
    GLuint _brickTexId;
    GLuint _grassTexId;
    GLuint _chessTexId;
    GLuint _myTexId;
    GLuint _cubeTexId;

    GLuint _sampler;
    GLuint _cubeTexSampler;
    GLuint _depthSampler;
    GLuint _depthSamplerLinear;

    GLuint _framebufferId;
    GLuint _depthTexId;
    unsigned int _fbWidth;
    unsigned int _fbHeight;

    bool _showDepthQuad;
    bool _isLinearSampler;
    bool _cullFrontFaces;
    bool _randomPoints;

    void initFramebuffer()
    {
        _fbWidth = 1024;
        _fbHeight = 1024;


        //Создаем фреймбуфер
        glGenFramebuffers(1, &_framebufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);


        ////Создаем текстуру, куда будем впоследствии копировать буфер глубины
        glGenTextures(1, &_depthTexId);
        glBindTexture(GL_TEXTURE_2D, _depthTexId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, _fbWidth, _fbHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthTexId, 0);


        //Указываем куда именно мы будем рендерить		
        GLenum buffers[] = { GL_NONE };
        glDrawBuffers(1, buffers);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void makeScene() override
    {
        Application::makeScene();

        _showDepthQuad = false;
        _isLinearSampler = false;
        _cullFrontFaces = false;
        _randomPoints = false;

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
        _quadShader.createProgram("shaders7/quadDepth.vert", "shaders7/quadDepth.frag");
        _renderToShadowMapShader.createProgram("shaders8/toshadow.vert", "shaders8/toshadow.frag");
        _commonWithShadowsShader.createProgram("shaders8/shadow.vert", "shaders8/shadow.frag");
        _commonWithShadowsShaderVar2.createProgram("shaders8/shadow.vert", "shaders8/shadow2.frag");

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

        GLfloat border[] = { 1.0f, 0.0f, 0.0f, 1.0f };

        glGenSamplers(1, &_depthSampler);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glSamplerParameterfv(_depthSampler, GL_TEXTURE_BORDER_COLOR, border);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glSamplerParameteri(_depthSampler, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

        glGenSamplers(1, &_depthSamplerLinear);
        glSamplerParameteri(_depthSamplerLinear, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_depthSamplerLinear, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_depthSamplerLinear, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glSamplerParameteri(_depthSamplerLinear, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glSamplerParameterfv(_depthSamplerLinear, GL_TEXTURE_BORDER_COLOR, border);
        glSamplerParameteri(_depthSamplerLinear, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        glSamplerParameteri(_depthSamplerLinear, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);


        //=========================================================
        //Инициализация фреймбуфера для рендера теневой карты

        initFramebuffer();
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

    virtual void handleKey(int key, int scancode, int action, int mods)
    {
        Application::handleKey(key, scancode, action, mods);

        if (action == GLFW_PRESS)
        {
            if (key == GLFW_KEY_Z)
            {
                _showDepthQuad = !_showDepthQuad;
            }
            else if (key == GLFW_KEY_L)
            {
                _isLinearSampler = !_isLinearSampler;
            }
            else if (key == GLFW_KEY_C)
            {
                _cullFrontFaces = !_cullFrontFaces;
            }
            else if (key == GLFW_KEY_1)
            {
                _randomPoints = !_randomPoints;
            }
        }
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
        drawToShadowMap(_lightCamera);
        drawToScreen(_randomPoints ? _commonWithShadowsShaderVar2 : _commonWithShadowsShader, _camera, _lightCamera);

        if (_showDepthQuad)
        {
            drawDebug();
        }
    }

    void drawToShadowMap(const CameraInfo& lightCamera)
    {
        //=========== Сначала подключаем фреймбуфер и рендерим в текстуру ==========
        glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);

        glViewport(0, 0, _fbWidth, _fbHeight);
        glClear(GL_DEPTH_BUFFER_BIT);

        _renderToShadowMapShader.use();
        _renderToShadowMapShader.setMat4Uniform("viewMatrix", lightCamera.viewMatrix);
        _renderToShadowMapShader.setMat4Uniform("projectionMatrix", lightCamera.projMatrix);

        if (_cullFrontFaces)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
        }

        drawScene(_renderToShadowMapShader, lightCamera);

        if (_cullFrontFaces)
        {
            glDisable(GL_CULL_FACE);
        }

        glUseProgram(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); //Отключаем фреймбуфер
    }

    void drawToScreen(const ShaderProgram& shader, const CameraInfo& camera, const CameraInfo& lightCamera)
    {
        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4Uniform("viewMatrix", camera.viewMatrix);
        shader.setMat4Uniform("projectionMatrix", camera.projMatrix);

        glm::vec3 lightPosCamSpace = glm::vec3(camera.viewMatrix * glm::vec4(_light.position, 1.0));

        shader.setVec3Uniform("light.pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
        shader.setVec3Uniform("light.La", _light.ambient);
        shader.setVec3Uniform("light.Ld", _light.diffuse);
        shader.setVec3Uniform("light.Ls", _light.specular);

        {
            shader.setMat4Uniform("lightViewMatrix", lightCamera.viewMatrix);
            shader.setMat4Uniform("lightProjectionMatrix", lightCamera.projMatrix);

            glm::mat4 projScaleBiasMatrix = glm::scale(glm::translate(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5)), glm::vec3(0.5, 0.5, 0.5));
            shader.setMat4Uniform("lightScaleBiasMatrix", projScaleBiasMatrix);
        }

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0
        glBindTexture(GL_TEXTURE_2D, _brickTexId);
        glBindSampler(0, _sampler);
        shader.setIntUniform("diffuseTex", 0);

        glActiveTexture(GL_TEXTURE1);  //текстурный юнит 1
        glBindTexture(GL_TEXTURE_2D, _depthTexId);
        glBindSampler(1, _isLinearSampler ? _depthSamplerLinear : _depthSampler);
        shader.setIntUniform("shadowTex", 1);


        drawScene(shader, camera);

        //Рисуем маркеры для всех источников света		
        {
            _markerShader.use();

            _markerShader.setMat4Uniform("mvpMatrix", camera.projMatrix * camera.viewMatrix * glm::translate(glm::mat4(1.0f), _light.position));
            _markerShader.setVec4Uniform("color", glm::vec4(_light.diffuse, 1.0f));
            _marker->draw();
        }

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
        glViewport(0, 0, 500, 500);

        _quadShader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _depthTexId);
        glBindSampler(0, _sampler);
        _quadShader.setIntUniform("tex", 0);

        _quad->draw();

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