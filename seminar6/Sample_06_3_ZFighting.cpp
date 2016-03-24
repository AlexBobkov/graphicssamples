#include <Application.hpp>
#include <LightInfo.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>
#include <Texture.hpp>

#include <iostream>
#include <sstream>
#include <vector>

/**
������ z-fighting
*/
class SampleApplication : public Application
{
public:
    MeshPtr _plane;

    MeshPtr _marker; //��� - ������ ��� ��������� �����

    //������������� ��������� ���������
    ShaderProgramPtr _commonShader;
    ShaderProgramPtr _markerShader;

    //���������� ��� ���������� ���������� ������ ��������� �����
    float _lr;
    float _phi;
    float _theta;

    LightInfo _light;

    TexturePtr _worldTex;
    TexturePtr _brickTex;

    GLuint _sampler;

    bool enableDepthTest;
    bool enablePolygonOffset;
    bool addZOffset;

    void makeScene() override
    {
        Application::makeScene();

        enableDepthTest = true;
        enablePolygonOffset = false;
        addZOffset = false;

        //=========================================================
        //�������� � �������� �����		

        _plane = makeGroundPlane(1.0f, 1.0f);

        _marker = makeSphere(0.1f);

        //=========================================================
        //������������� ��������

        _commonShader = std::make_shared<ShaderProgram>();
        _commonShader->createProgram("shaders/common.vert", "shaders/common.frag");

        _markerShader = std::make_shared<ShaderProgram>();
        _markerShader->createProgram("shaders/marker.vert", "shaders/marker.frag");

        //=========================================================
        //������������� �������� ���������� ��������
        _lr = 10.0;
        _phi = 2.65f;
        _theta = 0.48f;

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        _light.ambient = glm::vec3(0.2, 0.2, 0.2);
        _light.diffuse = glm::vec3(0.8, 0.8, 0.8);
        _light.specular = glm::vec3(1.0, 1.0, 1.0);

        //=========================================================
        //�������� � �������� �������
        _worldTex = loadTexture("images/earth_global.jpg");
        _brickTex = loadTexture("images/brick.jpg");

        //=========================================================
        //������������� ��������, �������, ������� ������ ��������� ������ �� ��������
        glGenSamplers(1, &_sampler);
        glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
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

            ImGui::Checkbox("Depth test", &enableDepthTest);
            ImGui::Checkbox("Polygon offset", &enablePolygonOffset);
            ImGui::Checkbox("Z offset", &addZOffset);
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
                enableDepthTest = !enableDepthTest;
            }
            else if (key == GLFW_KEY_2)
            {
                enablePolygonOffset = !enablePolygonOffset;
            }
            else if (key == GLFW_KEY_3)
            {
                addZOffset = !addZOffset;
            }
        }
    }

    void draw() override
    {
        //�������� ������� ������� ������ � ��������� �������
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //������� ������ ����� � ������� �� ����������� ���������� ����������� �����
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //====== ������ �������� ������� ����� ======
        _commonShader->use();

        //��������� �� ���������� �������� �������-����������
        _commonShader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        _commonShader->setMat4Uniform("projectionMatrix", _camera.projMatrix);

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        glm::vec3 lightPosCamSpace = glm::vec3(_camera.viewMatrix * glm::vec4(_light.position, 1.0));

        _commonShader->setVec3Uniform("light.pos", lightPosCamSpace); //�������� ��������� ��� � ������� ����������� ������
        _commonShader->setVec3Uniform("light.La", _light.ambient);
        _commonShader->setVec3Uniform("light.Ld", _light.diffuse);
        _commonShader->setVec3Uniform("light.Ls", _light.specular);

        glActiveTexture(GL_TEXTURE0);  //���������� ���� 0        
        glBindSampler(0, _sampler);
        _worldTex->bind();
        _commonShader->setIntUniform("diffuseTex", 0);

        //��������� �� ���������� ������� ������ ����� � ��������� ���������
        {
            glm::mat4 modelMatrix = _plane->modelMatrix();

            _commonShader->setMat4Uniform("modelMatrix", modelMatrix);
            _commonShader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * modelMatrix))));

            _plane->draw();
        }

        glActiveTexture(GL_TEXTURE0);  //���������� ���� 0        
        glBindSampler(0, _sampler);
        _brickTex->bind();
        _commonShader->setIntUniform("diffuseTex", 0);

        //��������� �� ���������� ������� ������ ����� � ��������� ���������
        {
            float zOffset = addZOffset ? 0.01f : 0.0f;

            glm::mat4 modelMatrix = glm::translate(_plane->modelMatrix(), glm::vec3(0.001f, 0.0f, zOffset));

            _commonShader->setMat4Uniform("modelMatrix", modelMatrix);
            _commonShader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * modelMatrix))));

            if (!enableDepthTest)
            {
                glDisable(GL_DEPTH_TEST);
            }
            if (enablePolygonOffset)
            {
                glEnable(GL_POLYGON_OFFSET_FILL);
                glPolygonOffset(-1.0f, -1.0f);
            }

            _plane->draw();

            if (enablePolygonOffset)
            {
                glPolygonOffset(0.0f, 0.0f);
                glDisable(GL_POLYGON_OFFSET_FILL);
            }
            if (!enableDepthTest)
            {
                glEnable(GL_DEPTH_TEST);
            }
        }

        //������ ������� ��� ���� ���������� �����		
        {
            _markerShader->use();

            _markerShader->setMat4Uniform("mvpMatrix", _camera.projMatrix * _camera.viewMatrix * glm::translate(glm::mat4(1.0f), _light.position));
            _markerShader->setVec4Uniform("color", glm::vec4(_light.diffuse, 1.0f));
            _marker->draw();
        }

        //����������� ������� � ��������� ���������
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