#include <Application.hpp>
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

/**
������ � ����������� ���������
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _sphere;
    MeshPtr _bunny;
    MeshPtr _ground;

    MeshPtr _marker; //��� - ������ ��� ��������� �����

    //������������� ��������� ���������
    ShaderProgramPtr _markerShader;
    ShaderProgramPtr _projectorShader;

    //���������� ��� ���������� ���������� ������ ��������� �����
    float _lr;
    float _phi;
    float _theta;

    LightInfo _light;

    TexturePtr _worldTex;
    TexturePtr _brickTex;

    GLuint _sampler;
    GLuint _cubeTexSampler;
    GLuint _projSampler;

    float _projR;
    float _projPhi;
    float _projTheta;

    CameraInfo _projCamera; //��� ���������� ���������� ����� ������������ �� �� ���������, ��� � ��� ����������� ������

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

        _marker = makeSphere(0.1f);

        //=========================================================
        //������������� ��������
        
        _projectorShader = std::make_shared<ShaderProgram>();
        _projectorShader->createProgram("shaders7/projector.vert", "shaders7/projector.frag");

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

        glGenSamplers(1, &_cubeTexSampler);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glGenSamplers(1, &_projSampler);
        glSamplerParameteri(_projSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glSamplerParameteri(_projSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glSamplerParameteri(_projSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glSamplerParameteri(_projSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        //=========================================================
        //������������� ���������

        _projR = 4.0f;
        _projPhi = 0.0;
        _projTheta = 1.0;

        glm::vec3 projPos = glm::vec3(glm::cos(_projPhi) * glm::cos(_projTheta), glm::sin(_projPhi) * glm::cos(_projTheta), glm::sin(_projTheta)) * _projR;

        _projCamera.viewMatrix = glm::lookAt(projPos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        _projCamera.projMatrix = glm::perspective(glm::radians(25.0f), 1.0f, 0.1f, 100.f);
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

            if (ImGui::CollapsingHeader("Projector"))
            {
                ImGui::SliderFloat("projector phi", &_projPhi, 0.0f, 2.0f * glm::pi<float>());
                ImGui::SliderFloat("projector theta", &_projTheta, 0.0f, glm::pi<float>());
            }
        }
        ImGui::End();
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
        _projectorShader->use();

        //��������� �� ���������� �������� �������-����������
        _projectorShader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        _projectorShader->setMat4Uniform("projectionMatrix", _camera.projMatrix);

        _light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
        glm::vec3 lightPosCamSpace = glm::vec3(_camera.viewMatrix * glm::vec4(_light.position, 1.0));

        _projectorShader->setVec3Uniform("light.pos", lightPosCamSpace); //�������� ��������� ��� � ������� ����������� ������
        _projectorShader->setVec3Uniform("light.La", _light.ambient);
        _projectorShader->setVec3Uniform("light.Ld", _light.diffuse);
        _projectorShader->setVec3Uniform("light.Ls", _light.specular);

        {
            glm::vec3 projPos = glm::vec3(glm::cos(_projPhi) * glm::cos(_projTheta), glm::sin(_projPhi) * glm::cos(_projTheta), glm::sin(_projTheta)) * _projR;

            _projCamera.viewMatrix = glm::lookAt(projPos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            _projCamera.projMatrix = glm::perspective(glm::radians(25.0f), 1.0f, 0.1f, 100.f);

            _projectorShader->setMat4Uniform("projViewMatrix", _projCamera.viewMatrix);
            _projectorShader->setMat4Uniform("projProjectionMatrix", _projCamera.projMatrix);

            glm::mat4 projScaleBiasMatrix = glm::scale(glm::translate(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5)), glm::vec3(0.5, 0.5, 0.5));
            _projectorShader->setMat4Uniform("projScaleBiasMatrix", projScaleBiasMatrix);
        }

        glActiveTexture(GL_TEXTURE0);  //���������� ���� 0
        _brickTex->bind();
        glBindSampler(0, _sampler);
        _projectorShader->setIntUniform("diffuseTex", 0);

        glActiveTexture(GL_TEXTURE1);  //���������� ���� 1
        _worldTex->bind();
        glBindSampler(1, _projSampler);
        _projectorShader->setIntUniform("projTex", 1);

        //��������� �� ���������� ������� ������ ����� � ��������� ���������
        {
            _projectorShader->setMat4Uniform("modelMatrix", _cube->modelMatrix());
            _projectorShader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _cube->modelMatrix()))));

            _cube->draw();
        }

        {
            _projectorShader->setMat4Uniform("modelMatrix", _sphere->modelMatrix());
            _projectorShader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _sphere->modelMatrix()))));

            _sphere->draw();
        }

        {
            _projectorShader->setMat4Uniform("modelMatrix", _bunny->modelMatrix());
            _projectorShader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _bunny->modelMatrix()))));

            _bunny->draw();
        }

        {
            _projectorShader->setMat4Uniform("modelMatrix", _ground->modelMatrix());
            _projectorShader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _ground->modelMatrix()))));

            _ground->draw();
        }

        //������ ������� ��� ���� ���������� �����		
        {
            _markerShader->use();

            _markerShader->setMat4Uniform("mvpMatrix", _camera.projMatrix * _camera.viewMatrix * glm::translate(glm::mat4(1.0f), _light.position));
            _markerShader->setVec4Uniform("color", glm::vec4(_light.diffuse, 1.0f));
            _marker->draw();
        }

        //������ ������ ��� ���������
        {
            _markerShader->use();

            glm::vec3 projPos = glm::vec3(glm::cos(_projPhi) * glm::cos(_projTheta), glm::sin(_projPhi) * glm::cos(_projTheta), glm::sin(_projTheta)) * _projR;

            _markerShader->setMat4Uniform("mvpMatrix", _camera.projMatrix * _camera.viewMatrix * glm::translate(glm::mat4(1.0f), projPos));
            _markerShader->setVec4Uniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
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