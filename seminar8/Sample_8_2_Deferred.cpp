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

float frand()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

//Удобная функция для вычисления цвета из линейной палитры от синего до красного
void getColorFromLinearPalette(float value, float& r, float& g, float& b)
{
    if (value < 0.25f)
    {
        r = 0.0f;
        g = value * 4.0f;
        b = 1.0f;
    }
    else if (value < 0.5f)
    {
        r = 0.0f;
        g = 1.0f;
        b = (0.5f - value) * 4.0f;
    }
    else if (value < 0.75f)
    {
        r = (value - 0.5f) * 4.0f;
        g = 1.0f;
        b = 0.0f;
    }
    else
    {
        r = 1.0f;
        g = (1.0f - value) * 4.0f;
        b = 0.0f;
    }
}

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
    ShaderProgram _quadDepthShader;
    ShaderProgram _quadColorShader;
    ShaderProgram _renderToGBufferShader;
    ShaderProgram _renderDeferredShader;

    //Переменные для управления положением одного источника света
    float _lr;
    float _phi;
    float _theta;

    LightInfo _light;

    TexturePtr _brickTex;

    GLuint _sampler;
    GLuint _cubeTexSampler;

    GLuint _framebufferId;

    GLuint _depthTexId;
    GLuint _normalsTexId;
    GLuint _diffuseTexId;

    unsigned int _fbWidth;
    unsigned int _fbHeight;

    bool _showDebugQuads;

    int Npositions;
    int Ncurrent;
    std::vector<glm::vec3> _positions;

    int Klights;
    int Kcurrent;
    std::vector<LightInfo> _lights;

    void initFramebuffer()
    {
        _fbWidth = 1024;
        _fbHeight = 1024;


        //Создаем фреймбуфер
        glGenFramebuffers(1, &_framebufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);


        //Создаем текстуру, куда будет осуществляться рендеринг нормалей
        glGenTextures(1, &_normalsTexId);
        glBindTexture(GL_TEXTURE_2D, _normalsTexId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, _fbWidth, _fbHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _normalsTexId, 0);

        //Создаем текстуру, куда будет осуществляться рендеринг диффузного цвета
        glGenTextures(1, &_diffuseTexId);
        glBindTexture(GL_TEXTURE_2D, _diffuseTexId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _fbWidth, _fbHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _diffuseTexId, 0);

        //Создаем текстуру, куда будем впоследствии копировать буфер глубины
        glGenTextures(1, &_depthTexId);
        glBindTexture(GL_TEXTURE_2D, _depthTexId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, _fbWidth, _fbHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthTexId, 0);


        //Указываем куда именно мы будем рендерить		
        GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, buffers);

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

        _showDebugQuads = false;

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

        _renderToGBufferShader.createProgram("shaders8/togbuffer.vert", "shaders8/togbuffer.frag");
        _renderDeferredShader.createProgram("shaders8/deferred.vert", "shaders8/deferred.frag");

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

        //=========================================================
        //Инициализация фреймбуфера для рендера теневой карты

        initFramebuffer();

        //=========================================================
        srand((int)(glfwGetTime() * 1000));

        Npositions = 100;
        Ncurrent = 0;
        float size = 20.0f;
        for (int i = 0; i < Npositions; i++)
        {
            _positions.push_back(glm::vec3(frand() * size - 0.5 * size, frand() * size - 0.5 * size, 0.0));
        }

        //=========================================================
        Klights = 100;
        Kcurrent = 0;
        size = 30.0f;
        for (int i = 0; i < Klights; i++)
        {
            LightInfo light;

            float r, g, b;
            getColorFromLinearPalette(frand(), r, g, b);

            light.position = glm::vec3(frand() * size - 0.5 * size, frand() * size - 0.5 * size, frand() * 10.0);
            light.ambient = glm::vec3(0.0 * r, 0.0 * g, 0.0 * b);
            light.diffuse = glm::vec3(0.4 * r, 0.4 * g, 0.4 * b);
            light.specular = glm::vec3(0.5, 0.5, 0.5);

            _lights.push_back(light);
        }
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
        TwAddVarRW(_bar, "Npositions", TW_TYPE_INT32, &Ncurrent, "step=1 min=0 max=100");
        TwAddVarRW(_bar, "Klights", TW_TYPE_INT32, &Kcurrent, "step=1 min=0 max=100");
    }

    virtual void handleKey(int key, int scancode, int action, int mods)
    {
        Application::handleKey(key, scancode, action, mods);

        if (action == GLFW_PRESS)
        {
            if (key == GLFW_KEY_Z)
            {
                _showDebugQuads = !_showDebugQuads;
            }
        }
    }

    void update()
    {
        Application::update();

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
    }

    void draw() override
    {
        drawToGBuffer(_camera);
        drawToScreen(_renderDeferredShader, _camera);

        if (_showDebugQuads)
        {
            drawDebug();
        }
    }

    void drawToGBuffer(const CameraInfo& camera)
    {
        //=========== Сначала подключаем фреймбуфер и рендерим в текстуру ==========
        glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);

        glViewport(0, 0, _fbWidth, _fbHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _renderToGBufferShader.use();
        _renderToGBufferShader.setMat4Uniform("viewMatrix", camera.viewMatrix);
        _renderToGBufferShader.setMat4Uniform("projectionMatrix", camera.projMatrix);

        glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0
        _brickTex->bind();
        glBindSampler(0, _sampler);
        _renderToGBufferShader.setIntUniform("diffuseTex", 0);

        drawScene(_renderToGBufferShader, camera);

        glUseProgram(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); //Отключаем фреймбуфер
    }

    void drawToScreen(const ShaderProgram& shader, const CameraInfo& camera)
    {
        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4Uniform("projMatrixInverse", glm::inverse(camera.projMatrix));

        glm::vec3 lightPosCamSpace = glm::vec3(camera.viewMatrix * glm::vec4(_light.position, 1.0));

        shader.setVec3Uniform("light.pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
        shader.setVec3Uniform("light.La", _light.ambient);
        shader.setVec3Uniform("light.Ld", _light.diffuse);
        shader.setVec3Uniform("light.Ls", _light.specular);

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

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        _quad->draw(); //main light

        for (unsigned int i = 0; i < Kcurrent; i++)
        {
            glm::vec3 lightPosCamSpace = glm::vec3(camera.viewMatrix * glm::vec4(_lights[i].position, 1.0));

            shader.setVec3Uniform("light.pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
            shader.setVec3Uniform("light.La", _lights[i].ambient);
            shader.setVec3Uniform("light.Ld", _lights[i].diffuse);
            shader.setVec3Uniform("light.Ls", _lights[i].specular);

            _quad->draw();
        }

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

        //Отсоединяем сэмплер и шейдерную программу
        glBindSampler(0, 0);
        glUseProgram(0);
    }

    void drawScene(const ShaderProgram& shader, const CameraInfo& camera)
    {
        shader.setMat4Uniform("modelMatrix", _cube->modelMatrix());
        shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _cube->modelMatrix()))));

        _cube->draw();

        shader.setMat4Uniform("modelMatrix", _sphere->modelMatrix());
        shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _sphere->modelMatrix()))));

        _sphere->draw();

        shader.setMat4Uniform("modelMatrix", _ground->modelMatrix());
        shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _ground->modelMatrix()))));

        _ground->draw();

        shader.setMat4Uniform("modelMatrix", _bunny->modelMatrix());
        shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _bunny->modelMatrix()))));

        _bunny->draw();

        for (unsigned int i = 0; i < Ncurrent; i++)
        {
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), _positions[i]);

            shader.setMat4Uniform("modelMatrix", modelMatrix);
            shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * modelMatrix))));

            _bunny->draw();
        }
    }

    void drawDebug()
    {
        glClear(GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, 500, 500);

        _quadDepthShader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _depthTexId);
        glBindSampler(0, _sampler);
        _quadDepthShader.setIntUniform("tex", 0);

        _quad->draw();

        glViewport(500, 0, 500, 500);

        _quadColorShader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _normalsTexId);
        glBindSampler(0, _sampler);
        _quadColorShader.setIntUniform("tex", 0);

        _quad->draw();

        glViewport(1000, 0, 500, 500);

        _quadColorShader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _diffuseTexId);
        glBindSampler(0, _sampler);
        _quadColorShader.setIntUniform("tex", 0);

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