#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
������� � ���������� ����� � �����������
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _sphere;
    MeshPtr _bunny;

    MeshPtr _marker; //������ ��� ��������� �����

    ShaderProgramPtr _shaderPerFragment;
    ShaderProgramPtr _markerShader;

    //���������� ��������� �����
    float _lr;
    float _phi;
    float _theta;

    //��������� ��������� �����
    glm::vec3 _lightAmbientColor;
    glm::vec3 _lightDiffuseColor;
    glm::vec3 _lightSpecularColor;
    float _attenuation;

    //��������� ���������
    glm::vec3 _rabbitAmbientColor;
    glm::vec3 _rabbitDiffuseColor;
    glm::vec3 _rabbitSpecularColor;
    float _rabbitShininess;

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

        _marker = makeSphere(0.1f);

        //=========================================================
        //������������� ��������

        _shaderPerFragment = std::make_shared<ShaderProgram>();
        _shaderPerFragment->createProgram("shaders4/specularAttenuationPointLightPerFragment.vert", "shaders4/specularAttenuationPointLightPerFragment.frag");

        _markerShader = std::make_shared<ShaderProgram>();
        _markerShader->createProgram("shaders4/marker.vert", "shaders4/marker.frag");

        //=========================================================
        //������������� �������� ���������� ��������
        _lr = 5.0;
        _phi = 0.0;
        _theta = glm::pi<float>() * 0.25f;

        _lightAmbientColor = glm::vec3(0.2, 0.2, 0.2);
        _lightDiffuseColor = glm::vec3(0.8, 0.8, 0.8);
        _lightSpecularColor = glm::vec3(1.0, 1.0, 1.0);
        _attenuation = 1.0f;

        //������������� ��������� �������
        _rabbitAmbientColor = glm::vec3(1.0, 1.0, 0.0);
        _rabbitDiffuseColor = glm::vec3(1.0, 1.0, 0.0);
        _rabbitSpecularColor = glm::vec3(1.0, 1.0, 1.0);
        _rabbitShininess = 128.0f;
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
                ImGui::ColorEdit3("ambient", glm::value_ptr(_lightAmbientColor));
                ImGui::ColorEdit3("diffuse", glm::value_ptr(_lightDiffuseColor));
                ImGui::ColorEdit3("specular", glm::value_ptr(_lightSpecularColor));

                ImGui::SliderFloat("radius", &_lr, 0.1f, 10.0f);
                ImGui::SliderFloat("phi", &_phi, 0.0f, 2.0f * glm::pi<float>());
                ImGui::SliderFloat("theta", &_theta, 0.0f, glm::pi<float>());

                ImGui::SliderFloat("attenuation", &_attenuation, 0.01f, 10.0f);
            }

            if (ImGui::CollapsingHeader("Rabbit material"))
            {
                ImGui::ColorEdit3("mat ambient", glm::value_ptr(_rabbitAmbientColor));
                ImGui::ColorEdit3("mat diffuse", glm::value_ptr(_rabbitDiffuseColor));
                ImGui::ColorEdit3("mat specular", glm::value_ptr(_rabbitSpecularColor));
                ImGui::SliderFloat("shininess", &_rabbitShininess, 0.1f, 255.0f);
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

        glm::vec3 lightPos = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * _lr;

        //���������� ������		
        _shaderPerFragment->use();

        //��������� �� ���������� �������� �������-����������
        _shaderPerFragment->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        _shaderPerFragment->setMat4Uniform("projectionMatrix", _camera.projMatrix);


        _shaderPerFragment->setVec3Uniform("light.pos", lightPos);
        _shaderPerFragment->setVec3Uniform("light.La", _lightAmbientColor);
        _shaderPerFragment->setVec3Uniform("light.Ld", _lightDiffuseColor);
        _shaderPerFragment->setVec3Uniform("light.Ls", _lightSpecularColor);
        _shaderPerFragment->setFloatUniform("light.attenuation", _attenuation);

        //��������� �� ���������� ������� ������ ����� � ��������� ���������
        {
            _shaderPerFragment->setMat4Uniform("modelMatrix", _cube->modelMatrix());
            _shaderPerFragment->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _cube->modelMatrix()))));

            _shaderPerFragment->setVec3Uniform("material.Ka", glm::vec3(0.0, 1.0, 0.0));
            _shaderPerFragment->setVec3Uniform("material.Kd", glm::vec3(0.0, 1.0, 0.0));
            _shaderPerFragment->setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
            _shaderPerFragment->setFloatUniform("material.shininess", _rabbitShininess);

            _cube->draw();
        }

        {
            _shaderPerFragment->setMat4Uniform("modelMatrix", _sphere->modelMatrix());
            _shaderPerFragment->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _sphere->modelMatrix()))));

            _shaderPerFragment->setVec3Uniform("material.Ka", glm::vec3(1.0, 1.0, 1.0));
            _shaderPerFragment->setVec3Uniform("material.Kd", glm::vec3(1.0, 1.0, 1.0));
            _shaderPerFragment->setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
            _shaderPerFragment->setFloatUniform("material.shininess", _rabbitShininess);

            _sphere->draw();
        }

        {
            _shaderPerFragment->setMat4Uniform("modelMatrix", _bunny->modelMatrix());
            _shaderPerFragment->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _bunny->modelMatrix()))));

            _shaderPerFragment->setVec3Uniform("material.Ka", glm::vec3(_rabbitAmbientColor));
            _shaderPerFragment->setVec3Uniform("material.Kd", glm::vec3(_rabbitDiffuseColor));
            _shaderPerFragment->setVec3Uniform("material.Ks", glm::vec3(_rabbitSpecularColor));
            _shaderPerFragment->setFloatUniform("material.shininess", _rabbitShininess);

            _bunny->draw();
        }

        //������ ������ ��� ��������� �����		
        {
            _markerShader->use();
            _markerShader->setMat4Uniform("mvpMatrix", _camera.projMatrix * _camera.viewMatrix * glm::translate(glm::mat4(1.0f), lightPos));
            _markerShader->setVec4Uniform("color", glm::vec4(_lightDiffuseColor, 1.0f));
            _marker->draw();
        }
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}