#include <Application.hpp>
#include <LightInfo.hpp>
#include <Framebuffer.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>
#include <Texture.hpp>

#include <iostream>
#include <sstream>
#include <vector>

/**
������ ������� ������������� - ����� ����������� � �����-����������
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _sphere;
    MeshPtr _bunny;
    MeshPtr _ground;

    MeshPtr _quad;

    //������������� ��������� ���������
    ShaderProgramPtr _quadDepthShader;
    ShaderProgramPtr _quadColorShader;
    ShaderProgramPtr _renderToShadowMapShader;
    ShaderProgramPtr _renderToGBufferShader;
    ShaderProgramPtr _renderDeferredShader;
    ShaderProgramPtr _gammaShader;

    //���������� ��� ���������� ���������� ������ ��������� �����
    float _lr;
    float _phi;
    float _theta;

    LightInfo _light;
    CameraInfo _lightCamera;

    TexturePtr _brickTex;
    TexturePtr _brickGammaTex;

    GLuint _sampler;
    GLuint _depthSampler;

    bool _applyEffect;
    bool _applyGammaTexture;

    bool _showGBufferDebug;
    bool _showShadowDebug;
    bool _showDeferredDebug;

    FramebufferPtr _gbufferFB;
    TexturePtr _depthTex;
    TexturePtr _normalsTex;
    TexturePtr _diffuseTex;

    FramebufferPtr _shadowFB;
    TexturePtr _shadowTex;

    FramebufferPtr _deferredFB;
    TexturePtr _deferredTex;

    //������ ������� ������
    int _oldWidth;
    int _oldHeight;

    SampleApplication() :
        Application(),
        _oldWidth(1024),
        _oldHeight(1024),
        _applyEffect(true),
        _applyGammaTexture(true),
        _showGBufferDebug(false),
        _showShadowDebug(false),
        _showDeferredDebug(false)
    {
    }

    void initFramebuffers()
    {
        //������� ���������� ��� ���������� � G-�����

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
        //������� ���������� ��� ���������� � ������� �����

        _shadowFB = std::make_shared<Framebuffer>(1024, 1024);

        _shadowTex = _shadowFB->addBuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT);

        _shadowFB->initDrawBuffers();

        if (!_shadowFB->valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }

        //=========================================================
        //������� ���������� ��� ����������� ������� ���������

        _deferredFB = std::make_shared<Framebuffer>(1024, 1024);

        _deferredTex = _deferredFB->addBuffer(GL_RGB8, GL_COLOR_ATTACHMENT0);

        _deferredFB->initDrawBuffers();

        if (!_deferredFB->valid())
        {
            std::cerr << "Failed to setup framebuffer\n";
            exit(1);
        }
    }

    void makeScene() override
    {
        Application::makeScene();

        //=========================================================
        //�������� � �������� �����		

        _cube = makeCube(0.5f);
        _cube->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f)));

        _sphere = makeSphere(0.5f);
        _sphere->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f)));

        _bunny = loadFromFile("models/bunny.obj");
        _bunny->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

        _ground = makeGroundPlane(5.0f, 2.0f);

        _quad = makeScreenAlignedQuad();

        //=========================================================
        //������������� ��������

        _quadDepthShader = std::make_shared<ShaderProgram>();
        _quadDepthShader->createProgram("shaders/quadDepth.vert", "shaders/quadDepth.frag");

        _quadColorShader = std::make_shared<ShaderProgram>();
        _quadColorShader->createProgram("shaders/quadColor.vert", "shaders/quadColor.frag");
        
        _renderToShadowMapShader = std::make_shared<ShaderProgram>();
        _renderToShadowMapShader->createProgram("shaders8/toshadow.vert", "shaders8/toshadow.frag");

        _renderToGBufferShader = std::make_shared<ShaderProgram>();
        _renderToGBufferShader->createProgram("shaders8/togbuffer.vert", "shaders8/togbuffer.frag");

        _renderDeferredShader = std::make_shared<ShaderProgram>();
        _renderDeferredShader->createProgram("shaders9/deferred.vert", "shaders9/deferred.frag");

        _gammaShader = std::make_shared<ShaderProgram>();
        _gammaShader->createProgram("shaders9/quad.vert", "shaders9/gamma.frag");

        //=========================================================
        //������������� �������� ���������� ��������
        _lr = 10.0;
        _phi = 0.0f;
        _theta = 0.48f;

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        _light.ambient = glm::vec3(0.2, 0.2, 0.2);
        _light.diffuse = glm::vec3(0.8, 0.8, 0.8);
        _light.specular = glm::vec3(1.0, 1.0, 1.0);

        //=========================================================
        //�������� � �������� �������
        _brickTex = loadTexture("images/brick.jpg", false);
        _brickGammaTex = loadTexture("images/brick.jpg", true);

        //=========================================================
        //������������� ��������, �������, ������� ������ ��������� ������ �� ��������
        glGenSamplers(1, &_sampler);
        glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

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
                ImGui::ColorEdit3("ambient", glm::value_ptr(_light.ambient));
                ImGui::ColorEdit3("diffuse", glm::value_ptr(_light.diffuse));
                ImGui::ColorEdit3("specular", glm::value_ptr(_light.specular));

                ImGui::SliderFloat("radius", &_lr, 0.1f, 10.0f);
                ImGui::SliderFloat("phi", &_phi, 0.0f, 2.0f * glm::pi<float>());
                ImGui::SliderFloat("theta", &_theta, 0.0f, glm::pi<float>());
            }

            ImGui::Checkbox("Apply gamma", &_applyEffect);
            ImGui::Checkbox("Use SRGB texture", &_applyGammaTexture);
            ImGui::Checkbox("Show G-buffer debug", &_showGBufferDebug);
            ImGui::Checkbox("Show shadow debug", &_showShadowDebug);
            ImGui::Checkbox("Show deferred debug", &_showDeferredDebug);
        }
        ImGui::End();
    }

    void handleKey(int key, int scancode, int action, int mods) override
    {
        Application::handleKey(key, scancode, action, mods);

        if (action == GLFW_PRESS)
        {
            if (key == GLFW_KEY_1)
            {
                _applyEffect = !_applyEffect;
            }
            else if (key == GLFW_KEY_2)
            {
                _applyGammaTexture = !_applyGammaTexture;
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

        //���� ������ ���� ���������, �� �������� ������� ������������ - ������������ ������ ��� ��������
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);
        if (width != _oldWidth || height != _oldHeight)
        {
            _gbufferFB->resize(width, height);
            _deferredFB->resize(width, height);

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

        //��������� ���������� ���������, ������ ����������� ����, � ��������� ���������� � ��������
        drawDeferred(_deferredFB, _renderDeferredShader, _camera, _lightCamera);

        //������� ���������� �������� �� �����, ������� �������� ������ �������������
        if (_applyEffect)
        {
            drawToScreen(_gammaShader);
        }
        else
        {
            drawToScreen(_quadColorShader);
        }

        //���������� ������ �������
        drawDebug();
    }

    void drawToGBuffer(const FramebufferPtr& fb, const ShaderProgramPtr& shader, const CameraInfo& camera)
    {
        fb->bind();

        glViewport(0, 0, fb->width(), fb->height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->use();
        shader->setMat4Uniform("viewMatrix", camera.viewMatrix);
        shader->setMat4Uniform("projectionMatrix", camera.projMatrix);

        glActiveTexture(GL_TEXTURE0);  //���������� ���� 0
        glBindSampler(0, _sampler);
        if (_applyGammaTexture)
        {
            _brickGammaTex->bind();
        }
        else
        {
            _brickTex->bind();
        }        
        shader->setIntUniform("diffuseTex", 0);

        drawScene(shader, camera);

        glUseProgram(0); //��������� ������

        fb->unbind(); //��������� ����������
    }

    void drawToShadowMap(const FramebufferPtr& fb, const ShaderProgramPtr& shader, const CameraInfo& lightCamera)
    {
        fb->bind();

        glViewport(0, 0, fb->width(), fb->height());
        glClear(GL_DEPTH_BUFFER_BIT);

        shader->use();
        shader->setMat4Uniform("lightViewMatrix", lightCamera.viewMatrix);
        shader->setMat4Uniform("lightProjectionMatrix", lightCamera.projMatrix);

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
        glCullFace(GL_FRONT);

        drawScene(shader, lightCamera);

        glDisable(GL_CULL_FACE);

        glUseProgram(0);

        fb->unbind();
    }

    void drawDeferred(const FramebufferPtr& fb, const ShaderProgramPtr& shader, const CameraInfo& camera, const CameraInfo& lightCamera)
    {
        fb->bind();

        glViewport(0, 0, fb->width(), fb->height());
        glClear(GL_COLOR_BUFFER_BIT);

        shader->use();
        shader->setMat4Uniform("viewMatrixInverse", glm::inverse(camera.viewMatrix));
        shader->setMat4Uniform("projMatrixInverse", glm::inverse(camera.projMatrix));

        glm::vec3 lightPosCamSpace = glm::vec3(camera.viewMatrix * glm::vec4(_light.position, 1.0));

        shader->setVec3Uniform("light.pos", lightPosCamSpace); //�������� ��������� ��� � ������� ����������� ������
        shader->setVec3Uniform("light.La", _light.ambient);
        shader->setVec3Uniform("light.Ld", _light.diffuse);
        shader->setVec3Uniform("light.Ls", _light.specular);

        shader->setMat4Uniform("lightViewMatrix", lightCamera.viewMatrix);
        shader->setMat4Uniform("lightProjectionMatrix", lightCamera.projMatrix);

        glm::mat4 projScaleBiasMatrix = glm::scale(glm::translate(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5)), glm::vec3(0.5, 0.5, 0.5));
        shader->setMat4Uniform("lightScaleBiasMatrix", projScaleBiasMatrix);

        glActiveTexture(GL_TEXTURE0);  //���������� ���� 0        
        glBindSampler(0, _sampler);
        _normalsTex->bind();
        shader->setIntUniform("normalsTex", 0);

        glActiveTexture(GL_TEXTURE1);  //���������� ���� 1        
        glBindSampler(1, _sampler);
        _diffuseTex->bind();
        shader->setIntUniform("diffuseTex", 1);

        glActiveTexture(GL_TEXTURE2);  //���������� ���� 2        
        glBindSampler(2, _sampler);
        _depthTex->bind();
        shader->setIntUniform("depthTex", 2);

        glActiveTexture(GL_TEXTURE3);  //���������� ���� 3        
        glBindSampler(3, _depthSampler);
        _shadowTex->bind();
        shader->setIntUniform("shadowTex", 3);

        _quad->draw(); //main light

        glUseProgram(0);

        fb->unbind();
    }

    void drawToScreen(const ShaderProgramPtr& shader)
    {
        //�������� ������� ������� ������ � ��������� �������
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->use();

        glActiveTexture(GL_TEXTURE0);        
        glBindSampler(0, _sampler);
        _deferredTex->bind();
        shader->setIntUniform("tex", 0);

        //glEnable(GL_FRAMEBUFFER_SRGB); //������������ ������� � ��������������

        _quad->draw();

        //glDisable(GL_FRAMEBUFFER_SRGB);

        //����������� ������� � ��������� ���������
        glBindSampler(0, 0);
        glUseProgram(0);
    }

    void drawScene(const ShaderProgramPtr& shader, const CameraInfo& camera)
    {
        shader->setMat4Uniform("modelMatrix", _cube->modelMatrix());
        shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _cube->modelMatrix()))));

        _cube->draw();

        shader->setMat4Uniform("modelMatrix", _sphere->modelMatrix());
        shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _sphere->modelMatrix()))));

        _sphere->draw();

        shader->setMat4Uniform("modelMatrix", _ground->modelMatrix());
        shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _ground->modelMatrix()))));

        _ground->draw();

        shader->setMat4Uniform("modelMatrix", _bunny->modelMatrix());
        shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * _bunny->modelMatrix()))));

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

        glBindSampler(0, 0);
        glUseProgram(0);
    }

    void drawQuad(const ShaderProgramPtr& shader, const TexturePtr& texture, GLint x, GLint y, GLint width, GLint height)
    {
        glViewport(x, y, width, height);

        shader->use();

        glActiveTexture(GL_TEXTURE0);        
        glBindSampler(0, _sampler);
        texture->bind();
        shader->setIntUniform("tex", 0);

        _quad->draw();
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}