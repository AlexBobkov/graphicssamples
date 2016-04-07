#include <Application.hpp>
#include <LightInfo.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>
#include <Texture.hpp>

#include <iostream>
#include <sstream>
#include <vector>

namespace
{
    float frand()
    {
        return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }

    //������� ������� ��� ���������� ����� �� �������� ������� �� ������ �� ��������
    glm::vec3 getColorFromLinearPalette(float value)
    {
        if (value < 0.25f)
        {
            return glm::vec3(0.0f, value * 4.0f, 1.0f);
        }
        else if (value < 0.5f)
        {
            return glm::vec3(0.0f, 1.0f, (0.5f - value) * 4.0f);
        }
        else if (value < 0.75f)
        {
            return glm::vec3((value - 0.5f) * 4.0f, 1.0f, 0.0f);
        }
        else
        {
            return glm::vec3(1.0f, (1.0f - value) * 4.0f, 0.0f);
        }
    }
}

/**
������ � ���������� �����������
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
    ShaderProgramPtr _renderToGBufferShader;
    ShaderProgramPtr _renderDeferredShader;

    //���������� ��� ���������� ���������� ������ ��������� �����
    float _lr;
    float _phi;
    float _theta;

    float _attenuation0;
    float _attenuation1;
    float _attenuation2;

    LightInfo _light;

    TexturePtr _brickTex;

    GLuint _sampler;

    GLuint _framebufferId;

    GLuint _depthTexId;
    GLuint _normalsTexId;
    GLuint _diffuseTexId;

    unsigned int _fbWidth;
    unsigned int _fbHeight;

    bool _showDebugQuads;

    int _Npositions;
    int _Ncurrent;
    std::vector<glm::vec3> _positions;

    int _Klights;
    int _Kcurrent;
    std::vector<LightInfo> _lights;

    SampleApplication() :
        Application(),
        _attenuation0(1.0f),
        _attenuation1(0.0f),
        _attenuation2(0.0f),
        _fbWidth(1024),
        _fbHeight(1024),
        _Npositions(100),
        _Ncurrent(0),
        _Klights(100),
        _Kcurrent(0),
        _showDebugQuads(false)
    {
    }

    void initFramebuffer()
    {
        _fbWidth = 1024;
        _fbHeight = 1024;

        //������� ����������
        glGenFramebuffers(1, &_framebufferId);
        glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);

        //----------------------------

        //������� ��������, ���� ����� �������������� ��������� ��������
        glGenTextures(1, &_normalsTexId);
        glBindTexture(GL_TEXTURE_2D, _normalsTexId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _fbWidth, _fbHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _normalsTexId, 0);

        //----------------------------

        //������� ��������, ���� ����� �������������� ��������� ���������� �����
        glGenTextures(1, &_diffuseTexId);
        glBindTexture(GL_TEXTURE_2D, _diffuseTexId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _fbWidth, _fbHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _diffuseTexId, 0);

        //----------------------------

        //������� ��������, ���� ����� ������������ ���������� ����� �������
        glGenTextures(1, &_depthTexId);
        glBindTexture(GL_TEXTURE_2D, _depthTexId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, _fbWidth, _fbHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depthTexId, 0);

        //----------------------------

        //��������� ���� ������ �� ����� ���������		
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
        //�������� � �������� �����		

        _cube = makeCube(0.5f);
        _cube->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f)));

        _sphere = makeSphere(0.5f);
        _sphere->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f)));

        _bunny = loadFromFile("models/bunny.obj");
        _bunny->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

        _ground = makeGroundPlane(10.0f, 2.0f);

        _quad = makeScreenAlignedQuad();

        //=========================================================
        //������������� ��������

        _quadDepthShader = std::make_shared<ShaderProgram>();
        _quadDepthShader->createProgram("shaders/quadDepth.vert", "shaders/quadDepth.frag");

        _quadColorShader = std::make_shared<ShaderProgram>();
        _quadColorShader->createProgram("shaders/quadColor.vert", "shaders/quadColor.frag");

        _renderToGBufferShader = std::make_shared<ShaderProgram>();
        _renderToGBufferShader->createProgram("shaders8/togbuffer.vert", "shaders8/togbuffer.frag");

        _renderDeferredShader = std::make_shared<ShaderProgram>();
        _renderDeferredShader->createProgram("shaders8/deferred.vert", "shaders8/deferred.frag");

        //=========================================================
        //������������� �������� ���������� ��������
        _lr = 10.0;
        _phi = 0.0f;
        _theta = 0.48f;

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        _light.ambient = glm::vec3(0.2, 0.2, 0.2);
        _light.diffuse = glm::vec3(0.8, 0.8, 0.8);
        _light.specular = glm::vec3(1.0, 1.0, 1.0);
        _light.attenuation0 = _attenuation0;
        _light.attenuation1 = _attenuation1;
        _light.attenuation2 = _attenuation2;

        //=========================================================
        //�������� � �������� �������
        _brickTex = loadTexture("images/brick.jpg");

        //=========================================================
        //������������� ��������, �������, ������� ������ ��������� ������ �� ��������
        glGenSamplers(1, &_sampler);
        glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

        //=========================================================
        //������������� ����������� ��� ������� ������� �����

        initFramebuffer();

        //=========================================================
        srand((int)(glfwGetTime() * 1000));

        float size = 20.0f;
        for (int i = 0; i < _Npositions; i++)
        {
            _positions.push_back(glm::vec3(frand() * size - 0.5 * size, frand() * size - 0.5 * size, 0.0));
        }

        //=========================================================
        size = 30.0f;
        for (int i = 0; i < _Klights; i++)
        {
            LightInfo light;
                        
            glm::vec3 color = getColorFromLinearPalette(frand());

            light.position = glm::vec3(frand() * size - 0.5 * size, frand() * size - 0.5 * size, frand() * 10.0);
            light.ambient = color * 0.0f;
            light.diffuse = color * 0.4f;
            light.specular = glm::vec3(0.5, 0.5, 0.5);
            light.attenuation0 = _attenuation0;
            light.attenuation1 = _attenuation1;
            light.attenuation2 = _attenuation2;

            _lights.push_back(light);
        }
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

                ImGui::SliderFloat("attenuation0", &_attenuation0, 1.0f, 10.0f);
                ImGui::SliderFloat("attenuation1", &_attenuation1, 0.0f, 1.0f);
                ImGui::SliderFloat("attenuation2", &_attenuation2, 0.0f, 1.0f);
            }

            ImGui::SliderInt("Mesh count", &_Ncurrent, 0, _Npositions);
            ImGui::SliderInt("Light count", &_Kcurrent, 0, _Klights);

            ImGui::Checkbox("Show depth quad", &_showDebugQuads);
        }
        ImGui::End();
    }

    void handleKey(int key, int scancode, int action, int mods) override
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
        //=========== ������� ���������� ���������� � �������� � �������� ==========
        glBindFramebuffer(GL_FRAMEBUFFER, _framebufferId);

        glViewport(0, 0, _fbWidth, _fbHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _renderToGBufferShader->use();
        _renderToGBufferShader->setMat4Uniform("viewMatrix", camera.viewMatrix);
        _renderToGBufferShader->setMat4Uniform("projectionMatrix", camera.projMatrix);

        glActiveTexture(GL_TEXTURE0);  //���������� ���� 0        
        glBindSampler(0, _sampler);
        _brickTex->bind();
        _renderToGBufferShader->setIntUniform("diffuseTex", 0);

        drawScene(_renderToGBufferShader, camera);

        glUseProgram(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); //��������� ����������
    }

    void drawToScreen(const ShaderProgramPtr& shader, const CameraInfo& camera)
    {
        //�������� ������� ������� ������ � ��������� �������
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //������� ������ ����� � ������� �� ����������� ���������� ����������� �����
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->use();
        shader->setMat4Uniform("projMatrixInverse", glm::inverse(camera.projMatrix));
                
        glActiveTexture(GL_TEXTURE0);  //���������� ���� 0
        glBindTexture(GL_TEXTURE_2D, _normalsTexId);
        glBindSampler(0, _sampler);
        shader->setIntUniform("normalsTex", 0);

        glActiveTexture(GL_TEXTURE1);  //���������� ���� 1
        glBindTexture(GL_TEXTURE_2D, _diffuseTexId);
        glBindSampler(1, _sampler);
        shader->setIntUniform("diffuseTex", 1);

        glActiveTexture(GL_TEXTURE2);  //���������� ���� 2
        glBindTexture(GL_TEXTURE_2D, _depthTexId);
        glBindSampler(2, _sampler);
        shader->setIntUniform("depthTex", 2);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        //��������� ��������� ������� ������ ��� ���� ���������� �����
        shader->setFloatUniform("light.a0", _attenuation0);
        shader->setFloatUniform("light.a1", _attenuation1);
        shader->setFloatUniform("light.a2", _attenuation2);

        glm::vec3 lightPosCamSpace = glm::vec3(camera.viewMatrix * glm::vec4(_light.position, 1.0));

        shader->setVec3Uniform("light.pos", lightPosCamSpace); //�������� ��������� ��� � ������� ����������� ������
        shader->setVec3Uniform("light.La", _light.ambient);
        shader->setVec3Uniform("light.Ld", _light.diffuse);
        shader->setVec3Uniform("light.Ls", _light.specular);        

        _quad->draw(); //main light

        for (int i = 0; i < _Kcurrent; i++)
        {
            glm::vec3 lightPosCamSpace = glm::vec3(camera.viewMatrix * glm::vec4(_lights[i].position, 1.0));

            shader->setVec3Uniform("light.pos", lightPosCamSpace); //�������� ��������� ��� � ������� ����������� ������
            shader->setVec3Uniform("light.La", _lights[i].ambient);
            shader->setVec3Uniform("light.Ld", _lights[i].diffuse);
            shader->setVec3Uniform("light.Ls", _lights[i].specular);

            _quad->draw();
        }

        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);

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

        for (int i = 0; i < _Ncurrent; i++)
        {
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), _positions[i]);

            shader->setMat4Uniform("modelMatrix", modelMatrix);
            shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(camera.viewMatrix * modelMatrix))));

            _bunny->draw();
        }
    }

    void drawDebug()
    {
        glClear(GL_DEPTH_BUFFER_BIT);

        glViewport(0, 0, 500, 500);

        _quadDepthShader->use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _depthTexId);
        glBindSampler(0, _sampler);
        _quadDepthShader->setIntUniform("tex", 0);

        _quad->draw();

        //----------------------------

        glViewport(500, 0, 500, 500);

        _quadColorShader->use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _normalsTexId);
        glBindSampler(0, _sampler);
        _quadColorShader->setIntUniform("tex", 0);

        _quad->draw();

        //----------------------------

        glViewport(1000, 0, 500, 500);

        _quadColorShader->use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _diffuseTexId);
        glBindSampler(0, _sampler);
        _quadColorShader->setIntUniform("tex", 0);

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