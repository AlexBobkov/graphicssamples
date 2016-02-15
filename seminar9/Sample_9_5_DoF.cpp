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
    glm::vec3 position; //����� ����� ������� ���������� � ������� ������� ���������, � ��� ����������� � �������-���������� �������������� � ������� ����������� ������
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

float frand()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

/**
������ ������� "������� ��������"
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _sphere;
    MeshPtr _bunny;
    MeshPtr _ground;

    MeshPtr _quad;

    MeshPtr _marker; //��� - ������ ��� ��������� �����

    //������������� ��������� ���������
    ShaderProgram _commonShader;
    ShaderProgram _markerShader;
    ShaderProgram _skyboxShader;
    ShaderProgram _quadDepthShader;
    ShaderProgram _quadColorShader;
    ShaderProgram _renderToShadowMapShader;
    ShaderProgram _renderToGBufferShader;
    ShaderProgram _renderDeferredShader;
    ShaderProgram _renderDeferredWithSSAOShader;
    ShaderProgram _gammaShader;
    ShaderProgram _brightShader;
    ShaderProgram _horizBlurShader;
    ShaderProgram _vertBlurShader;
    ShaderProgram _toneMappingShader;
    ShaderProgram _ssaoShader;
    ShaderProgram _dofShader;

    //���������� ��� ���������� ���������� ������ ��������� �����
    float _lr;
    float _phi;
    float _theta;

    float _lightIntensity;
    LightInfo _light;
    CameraInfo _lightCamera;

    TexturePtr _brickTex;
    TexturePtr _rotateTex;

    GLuint _sampler;
    GLuint _repeatSampler;
    GLuint _cubeTexSampler;
    GLuint _depthSampler;

    bool _applyEffect;

    bool _showGBufferDebug;
    bool _showShadowDebug;
    bool _showDeferredDebug;
    bool _showHDRDebug;
    bool _showSSAODebug;
    bool _showDoFDebug;

    float _exposure; //�������� ��������� ToneMapping
    float _focalDistance;
    float _focalRange;

    FramebufferPtr _gbufferFB;
    TexturePtr _depthTex;
    TexturePtr _normalsTex;
    TexturePtr _diffuseTex;

    FramebufferPtr _shadowFB;
    TexturePtr _shadowTex;

    FramebufferPtr _deferredFB;
    TexturePtr _deferredTex;

    FramebufferPtr _brightFB;
    TexturePtr _brightTex;

    FramebufferPtr _horizBlurFB;
    TexturePtr _horizBlurTex;

    FramebufferPtr _vertBlurFB;
    TexturePtr _vertBlurTex;

    FramebufferPtr _toneMappingFB;
    TexturePtr _toneMappingTex;

    FramebufferPtr _ssaoFB;
    TexturePtr _ssaoTex;

    FramebufferPtr _horizBlurDofFB;
    TexturePtr _horizBlurDofTex;

    FramebufferPtr _vertBlurDofFB;
    TexturePtr _vertBlurDofTex;

    FramebufferPtr _dofFB;
    TexturePtr _dofTex;

    //������ ������� ������
    int _oldWidth;
    int _oldHeight;

    void initFramebuffers()
    {
        _gbufferFB = std::make_shared<Framebuffer>(1024, 1024);

        _normalsTex = _gbufferFB->addBuffer(GL_RGB16F, GL_COLOR_ATTACHMENT0);
        _diffuseTex = _gbufferFB->addBuffer(GL_RGB8, GL_COLOR_ATTACHMENT1);
        _depthTex = _gbufferFB->addBuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT);

        _gbufferFB->initDrawBuffers();

        if (!_gbufferFB->valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        //=========================================================

        _shadowFB = std::make_shared<Framebuffer>(1024, 1024);

        _shadowTex = _shadowFB->addBuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT);

        _shadowFB->initDrawBuffers();

        if (!_shadowFB->valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        //=========================================================

        _deferredFB = std::make_shared<Framebuffer>(1024, 1024);

        _deferredTex = _deferredFB->addBuffer(GL_RGB32F, GL_COLOR_ATTACHMENT0);

        _deferredFB->initDrawBuffers();

        if (!_deferredFB->valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        //=========================================================

        _brightFB = std::make_shared<Framebuffer>(512, 512); //� 2 ���� ������

        _brightTex = _brightFB->addBuffer(GL_RGB32F, GL_COLOR_ATTACHMENT0);

        _brightFB->initDrawBuffers();

        if (!_brightFB->valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        //=========================================================

        _horizBlurFB = std::make_shared<Framebuffer>(512, 512); //� 2 ���� ������

        _horizBlurTex = _horizBlurFB->addBuffer(GL_RGB32F, GL_COLOR_ATTACHMENT0);

        _horizBlurFB->initDrawBuffers();

        if (!_horizBlurFB->valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        //=========================================================

        _vertBlurFB = std::make_shared<Framebuffer>(512, 512); //� 2 ���� ������

        _vertBlurTex = _vertBlurFB->addBuffer(GL_RGB32F, GL_COLOR_ATTACHMENT0);

        _vertBlurFB->initDrawBuffers();

        if (!_vertBlurFB->valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        //=========================================================

        _toneMappingFB = std::make_shared<Framebuffer>(1024, 1024);

        _toneMappingTex = _toneMappingFB->addBuffer(GL_RGB8, GL_COLOR_ATTACHMENT0);

        _toneMappingFB->initDrawBuffers();

        if (!_toneMappingFB->valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        //=========================================================

        _ssaoFB = std::make_shared<Framebuffer>(1024, 1024);

        _ssaoTex = _ssaoFB->addBuffer(GL_RGB8, GL_COLOR_ATTACHMENT0);

        _ssaoFB->initDrawBuffers();

        if (!_ssaoFB->valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        //=========================================================

        _horizBlurDofFB = std::make_shared<Framebuffer>(512, 512);

        _horizBlurDofTex = _horizBlurDofFB->addBuffer(GL_RGB8, GL_COLOR_ATTACHMENT0);

        _horizBlurDofFB->initDrawBuffers();

        if (!_horizBlurDofFB->valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        //=========================================================

        _vertBlurDofFB = std::make_shared<Framebuffer>(512, 512);

        _vertBlurDofTex = _vertBlurDofFB->addBuffer(GL_RGB8, GL_COLOR_ATTACHMENT0);

        _vertBlurDofFB->initDrawBuffers();

        if (!_vertBlurDofFB->valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        //=========================================================

        _dofFB = std::make_shared<Framebuffer>(1024, 1024);

        _dofTex = _dofFB->addBuffer(GL_RGB8, GL_COLOR_ATTACHMENT0);

        _dofFB->initDrawBuffers();

        if (!_dofFB->valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }
    }

    void makeScene() override
    {
        Application::makeScene();

        _applyEffect = true;
        _showGBufferDebug = false;
        _showShadowDebug = false;
        _showDeferredDebug = false;
        _showHDRDebug = false;
        _showSSAODebug = false;
        _showDoFDebug = false;

        //=========================================================
        //�������� � �������� �����		

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
        //������������� ��������

        _commonShader.createProgram("shaders6/common.vert", "shaders6/common.frag");
        _markerShader.createProgram("shaders4/marker.vert", "shaders4/marker.frag");
        _skyboxShader.createProgram("shaders6/skybox.vert", "shaders6/skybox.frag");
        _quadDepthShader.createProgram("shaders7/quadDepth.vert", "shaders7/quadDepth.frag");
        _quadColorShader.createProgram("shaders7/quadColor.vert", "shaders7/quadColor.frag");
        _renderToShadowMapShader.createProgram("shaders8/toshadow.vert", "shaders8/toshadow.frag");
        _renderToGBufferShader.createProgram("shaders8/togbuffer.vert", "shaders8/togbuffer.frag");
        _renderDeferredShader.createProgram("shaders9/deferred.vert", "shaders9/deferred.frag");
        _renderDeferredWithSSAOShader.createProgram("shaders9/deferred.vert", "shaders9/deferredWithSSAO.frag");
        _gammaShader.createProgram("shaders9/quad.vert", "shaders9/gamma.frag");
        _brightShader.createProgram("shaders9/quad.vert", "shaders9/bright.frag");
        _horizBlurShader.createProgram("shaders9/quad.vert", "shaders9/horizblur.frag");
        _vertBlurShader.createProgram("shaders9/quad.vert", "shaders9/vertblur.frag");
        _toneMappingShader.createProgram("shaders9/quad.vert", "shaders9/tonemapping.frag");
        _ssaoShader.createProgram("shaders9/quad.vert", "shaders9/ssao.frag");
        _dofShader.createProgram("shaders9/quad.vert", "shaders9/dof.frag");

        //=========================================================
        //������������� �������� ���������� ��������
        _lr = 10.0;
        _phi = 0.0f;
        _theta = 0.48f;

        _lightIntensity = 1.0f;

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        _light.ambient = glm::vec3(0.2, 0.2, 0.2);
        _light.diffuse = glm::vec3(0.8, 0.8, 0.8);
        _light.specular = glm::vec3(1.0, 1.0, 1.0);

        //=========================================================
        //�������� � �������� �������
        _brickTex = loadTexture("images/brick.jpg", true); //sRGB
        _rotateTex = loadTexture("images/rotate.png");

        //=========================================================
        //������������� ��������, �������, ������� ������ ��������� ������ �� ��������
        glGenSamplers(1, &_sampler);
        glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenSamplers(1, &_repeatSampler);
        glSamplerParameteri(_repeatSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_repeatSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_repeatSampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_repeatSampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

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
        //������������� ����������� ��� ������� ������� �����

        initFramebuffers();

        _exposure = 1.0f;

        _focalDistance = 5.0f;
        _focalRange = 1.0f;
    }

    void initGUI() override
    {
        Application::initGUI();

        TwAddVarRW(_bar, "r", TW_TYPE_FLOAT, &_lr, "group=Light step=0.01 min=0.1 max=100.0");
        TwAddVarRW(_bar, "phi", TW_TYPE_FLOAT, &_phi, "group=Light step=0.01 min=0.0 max=6.28");
        TwAddVarRW(_bar, "theta", TW_TYPE_FLOAT, &_theta, "group=Light step=0.01 min=-1.57 max=1.57");
        TwAddVarRW(_bar, "intensity", TW_TYPE_FLOAT, &_lightIntensity, "group=Light step=0.01 min=0.0 max=100.0");
        TwAddVarRW(_bar, "La", TW_TYPE_COLOR3F, &_light.ambient, "group=Light label='ambient'");
        TwAddVarRW(_bar, "Ld", TW_TYPE_COLOR3F, &_light.diffuse, "group=Light label='diffuse'");
        TwAddVarRW(_bar, "Ls", TW_TYPE_COLOR3F, &_light.specular, "group=Light label='specular'");
        TwAddVarRO(_bar, "DoF", TW_TYPE_BOOLCPP, &_applyEffect, "");
        TwAddVarRW(_bar, "Exposure", TW_TYPE_FLOAT, &_exposure, "min=0.01 max=100.0 step=0.01");
        TwAddVarRW(_bar, "Focal distance", TW_TYPE_FLOAT, &_focalDistance, "min=1.0 max=100.0 step=0.1");
        TwAddVarRW(_bar, "Focal range", TW_TYPE_FLOAT, &_focalRange, "min=0.1 max=100.0 step=0.1");
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
            else if (key == GLFW_KEY_V)
            {
                _showHDRDebug = !_showHDRDebug;
            }
            else if (key == GLFW_KEY_B)
            {
                _showSSAODebug = !_showSSAODebug;
            }
            else if (key == GLFW_KEY_N)
            {
                _showDoFDebug = !_showDoFDebug;
            }
        }
    }

    void update()
    {
        Application::update();

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        _lightCamera.viewMatrix = glm::lookAt(_light.position, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        _lightCamera.projMatrix = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 30.f);

        //���� ������ ���� ���������, �� �������� ������� ������������ - ������������ ������ ��� ��������

        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);
        if (width != _oldWidth || height != _oldHeight)
        {
            _gbufferFB->resize(width, height);
            _deferredFB->resize(width, height);

            _brightFB->resize(width / 2, height / 2);
            _horizBlurFB->resize(width / 2, height / 2);
            _vertBlurFB->resize(width / 2, height / 2);
            _toneMappingFB->resize(width, height);

            _ssaoFB->resize(width, height);

            _horizBlurDofFB->resize(width / 2, height / 2);
            _vertBlurDofFB->resize(width / 2, height / 2);
            _dofFB->resize(width, height);

            _oldWidth = width;
            _oldHeight = height;
        }
    }

    void draw() override
    {
        //�������� ��������� ����� � G-�����
        drawToGBuffer(_gbufferFB, _renderToGBufferShader, _camera);

        //�������� ��������� ����� � ������� ����� � ������� ��������� �����
        drawToShadowMap(_shadowFB, _renderToShadowMapShader, _lightCamera);

        //���������� Ambient Occlusion ��������
        drawSSAO(_ssaoFB, _ssaoShader, _camera);


        //��������� ���������� ���������, ������ ����������� ����, � ��������� ���������� � ��������
        drawDeferred(_deferredFB, _renderDeferredWithSSAOShader, _camera, _lightCamera);


        //�������� �������� � ������ ���������
        drawProcessTexture(_brightFB, _brightShader, _deferredTex, _deferredFB->width(), _deferredFB->height());

        //��������� �������� �������� � ������ ���������
        drawProcessTexture(_horizBlurFB, _horizBlurShader, _brightTex, _brightFB->width(), _brightFB->height());
        drawProcessTexture(_vertBlurFB, _vertBlurShader, _horizBlurTex, _horizBlurFB->width(), _horizBlurFB->height());

        drawToneMapping(_toneMappingFB, _toneMappingShader);

        drawProcessTexture(_horizBlurDofFB, _horizBlurShader, _toneMappingTex, _toneMappingFB->width(), _toneMappingFB->height());
        drawProcessTexture(_vertBlurDofFB, _vertBlurShader, _horizBlurDofTex, _horizBlurDofFB->width(), _horizBlurDofFB->height());

        drawDoF(_dofFB, _dofShader, _camera);

        if (_applyEffect)
        {
            drawToScreen(_gammaShader, _dofTex);
        }
        else
        {
            drawToScreen(_gammaShader, _toneMappingTex);
        }

        //���������� ������ �������
        drawDebug();
    }

    void drawToGBuffer(const FramebufferPtr& fb, const ShaderProgram& shader, const CameraInfo& camera)
    {
        fb->bind();

        glViewport(0, 0, fb->width(), fb->height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4Uniform("viewMatrix", camera.viewMatrix);
        shader.setMat4Uniform("projectionMatrix", camera.projMatrix);

        glActiveTexture(GL_TEXTURE0);  //���������� ���� 0
        _brickTex->bind();
        glBindSampler(0, _repeatSampler);
        shader.setIntUniform("diffuseTex", 0);

        drawScene(shader, camera);

        glUseProgram(0); //��������� ������

        fb->unbind(); //��������� ����������
    }

    void drawToShadowMap(const FramebufferPtr& fb, const ShaderProgram& shader, const CameraInfo& lightCamera)
    {
        fb->bind();

        glViewport(0, 0, fb->width(), fb->height());
        glClear(GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4Uniform("viewMatrix", lightCamera.viewMatrix);
        shader.setMat4Uniform("projectionMatrix", lightCamera.projMatrix);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        drawScene(shader, lightCamera);

        glDisable(GL_CULL_FACE);

        glUseProgram(0);

        fb->unbind();
    }

    void drawDeferred(const FramebufferPtr& fb, const ShaderProgram& shader, const CameraInfo& camera, const CameraInfo& lightCamera)
    {
        fb->bind();

        glViewport(0, 0, fb->width(), fb->height());
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        shader.setMat4Uniform("viewMatrixInverse", glm::inverse(camera.viewMatrix));
        shader.setMat4Uniform("projMatrixInverse", glm::inverse(camera.projMatrix));

        glm::vec3 lightPosCamSpace = glm::vec3(camera.viewMatrix * glm::vec4(_light.position, 1.0));

        shader.setVec3Uniform("light.pos", lightPosCamSpace); //�������� ��������� ��� � ������� ����������� ������
        shader.setVec3Uniform("light.La", _light.ambient * _lightIntensity);
        shader.setVec3Uniform("light.Ld", _light.diffuse * _lightIntensity);
        shader.setVec3Uniform("light.Ls", _light.specular * _lightIntensity);

        shader.setMat4Uniform("lightViewMatrix", lightCamera.viewMatrix);
        shader.setMat4Uniform("lightProjectionMatrix", lightCamera.projMatrix);

        glm::mat4 projScaleBiasMatrix = glm::scale(glm::translate(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5)), glm::vec3(0.5, 0.5, 0.5));
        shader.setMat4Uniform("lightScaleBiasMatrix", projScaleBiasMatrix);

        glActiveTexture(GL_TEXTURE0);  //���������� ���� 0
        _normalsTex->bind();
        glBindSampler(0, _sampler);
        shader.setIntUniform("normalsTex", 0);

        glActiveTexture(GL_TEXTURE1);  //���������� ���� 1
        _diffuseTex->bind();
        glBindSampler(1, _sampler);
        shader.setIntUniform("diffuseTex", 1);

        glActiveTexture(GL_TEXTURE2);  //���������� ���� 2
        _depthTex->bind();
        glBindSampler(2, _sampler);
        shader.setIntUniform("depthTex", 2);

        glActiveTexture(GL_TEXTURE3);  //���������� ���� 3
        _shadowTex->bind();
        glBindSampler(3, _depthSampler);
        shader.setIntUniform("shadowTex", 3);

        glActiveTexture(GL_TEXTURE4);  //���������� ���� 4
        _ssaoTex->bind();
        glBindSampler(4, _sampler);
        shader.setIntUniform("ssaoTex", 4);

        _quad->draw();

        glUseProgram(0);

        fb->unbind();
    }

    void drawSSAO(const FramebufferPtr& fb, const ShaderProgram& shader, const CameraInfo& camera)
    {
        fb->bind();

        glViewport(0, 0, fb->width(), fb->height());
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        shader.setMat4Uniform("projMatrix", camera.projMatrix);
        shader.setMat4Uniform("projMatrixInverse", glm::inverse(camera.projMatrix));

        glActiveTexture(GL_TEXTURE0);  //���������� ���� 0
        _depthTex->bind();
        glBindSampler(0, _sampler);
        shader.setIntUniform("depthTex", 0);

        glActiveTexture(GL_TEXTURE1);  //���������� ���� 1
        _rotateTex->bind();
        glBindSampler(1, _repeatSampler);
        shader.setIntUniform("rotateTex", 1);

        _quad->draw();

        glUseProgram(0);

        fb->unbind();
    }

    void drawDoF(const FramebufferPtr& fb, const ShaderProgram& shader, const CameraInfo& camera)
    {
        fb->bind();

        glViewport(0, 0, fb->width(), fb->height());
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        shader.setMat4Uniform("projMatrixInverse", glm::inverse(camera.projMatrix));

        shader.setFloatUniform("focalDistance", _focalDistance);
        shader.setFloatUniform("focalRange", _focalRange);

        glActiveTexture(GL_TEXTURE0);  //���������� ���� 0
        _toneMappingTex->bind();
        glBindSampler(0, _sampler);
        shader.setIntUniform("tex", 0);

        glActiveTexture(GL_TEXTURE1);  //���������� ���� 1
        _vertBlurDofTex->bind();
        glBindSampler(1, _sampler);
        shader.setIntUniform("blurTex", 1);

        glActiveTexture(GL_TEXTURE2);  //���������� ���� 2
        _depthTex->bind();
        glBindSampler(2, _sampler);
        shader.setIntUniform("depthTex", 2);

        _quad->draw();

        glUseProgram(0);

        fb->unbind();
    }

    void drawProcessTexture(const FramebufferPtr& fb, const ShaderProgram& shader, const TexturePtr& inputTexture, int inputTexWidth, int inputTexHeight)
    {
        fb->bind();

        glViewport(0, 0, fb->width(), fb->height());
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        shader.setVec2Uniform("texSize", glm::vec2(inputTexWidth, inputTexHeight));

        glActiveTexture(GL_TEXTURE0);
        inputTexture->bind();
        glBindSampler(0, _sampler);
        shader.setIntUniform("tex", 0);

        _quad->draw();

        glUseProgram(0);

        fb->unbind();
    }

    void drawToneMapping(const FramebufferPtr& fb, const ShaderProgram& shader)
    {
        fb->bind();

        glViewport(0, 0, fb->width(), fb->height());
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        shader.setFloatUniform("exposure", _exposure);

        glActiveTexture(GL_TEXTURE0);
        _deferredTex->bind();
        glBindSampler(0, _sampler);
        shader.setIntUniform("tex", 0);

        glActiveTexture(GL_TEXTURE1);
        _vertBlurTex->bind();
        glBindSampler(1, _sampler);
        shader.setIntUniform("bloomTex", 1);

        _quad->draw();

        glUseProgram(0);

        fb->unbind();
    }

    void drawToScreen(const ShaderProgram& shader, const TexturePtr& inputTexture)
    {
        //�������� ������� ������� ������ � ��������� �������
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glActiveTexture(GL_TEXTURE0);
        inputTexture->bind();
        glBindSampler(0, _sampler);
        shader.setIntUniform("tex", 0);

        _quad->draw();

        //����������� ������� � ��������� ���������
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
            drawQuad(_quadDepthShader, _depthTex, 0, 0, size, size);
            drawQuad(_quadColorShader, _normalsTex, size, 0, size, size);
            drawQuad(_quadColorShader, _diffuseTex, size * 2, 0, size, size);
        }
        else if (_showShadowDebug)
        {
            drawQuad(_quadDepthShader, _shadowTex, 0, 0, size, size);
        }
        else if (_showDeferredDebug)
        {
            drawQuad(_quadColorShader, _deferredTex, 0, 0, size, size);
        }
        else if (_showHDRDebug)
        {
            drawQuad(_quadColorShader, _brightTex, 0, 0, size, size);
            drawQuad(_quadColorShader, _horizBlurTex, size, 0, size, size);
            drawQuad(_quadColorShader, _vertBlurTex, size * 2, 0, size, size);
        }
        else if (_showSSAODebug)
        {
            drawQuad(_quadColorShader, _ssaoTex, 0, 0, size, size);
        }
        else if (_showDoFDebug)
        {
            drawQuad(_quadColorShader, _vertBlurDofTex, 0, 0, size, size);
        }

        glBindSampler(0, 0);
        glUseProgram(0);
    }

    void drawQuad(const ShaderProgram& shader, const TexturePtr& texture, GLint x, GLint y, GLint width, GLint height)
    {
        glViewport(x, y, width, height);

        shader.use();

        glActiveTexture(GL_TEXTURE0);
        texture->bind();
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