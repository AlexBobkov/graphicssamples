#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
������������ �������� �����
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _sphere;
    MeshPtr _bunny;

    ShaderProgramPtr _shader;

    float _phi;
    float _theta;

    //����������, ������� �������� ��������, ������� ����� �������� � uniform-���������� ��������
    glm::vec3 _lightAmbientColor;
    glm::vec3 _lightDiffuseColor;

    glm::vec3 _rabbitAmbientColor;
    glm::vec3 _rabbitDiffuseColor;

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

        //=========================================================
        //������������� ��������

        _shader = std::make_shared<ShaderProgram>();
        _shader->createProgram("shaders4/diffuseDirectionalLight.vert", "shaders4/diffuseDirectionalLight.frag");

        //=========================================================
        //������������� �������� ���������� ��������
        _phi = 0.0;
        _theta = glm::pi<float>() * 0.25f;

        _lightAmbientColor = glm::vec3(0.2, 0.2, 0.2);
        _lightDiffuseColor = glm::vec3(0.8, 0.8, 0.8);

        //������������� ��������� �������
        _rabbitAmbientColor = glm::vec3(1.0, 1.0, 0.0);
        _rabbitDiffuseColor = glm::vec3(1.0, 1.0, 0.0);
    }

    void initGUI() override
    {
        Application::initGUI();

        TwAddVarRW(_bar, "phi", TW_TYPE_FLOAT, &_phi, "group=Light step=0.01 min=0.0 max=6.28");
        TwAddVarRW(_bar, "theta", TW_TYPE_FLOAT, &_theta, "group=Light step=0.01 min=-1.57 max=1.57");
        TwAddVarRW(_bar, "La", TW_TYPE_COLOR3F, &_lightAmbientColor, "group=Light label='ambient'");
        TwAddVarRW(_bar, "Ld", TW_TYPE_COLOR3F, &_lightDiffuseColor, "group=Light label='diffuse'");
        TwAddVarRW(_bar, "Ka", TW_TYPE_COLOR3F, &_rabbitAmbientColor, "group='Rabbit material' label='ambient'");
        TwAddVarRW(_bar, "Kd", TW_TYPE_COLOR3F, &_rabbitDiffuseColor, "group='Rabbit material' label='diffuse'");
    }

    void draw() override
    {
        //�������� ������� ������� ������ � ��������� �������
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //������� ������ ����� � ������� �� ����������� ���������� ����������� �����
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //���������� ������		
        _shader->use();

        //��������� �� ���������� �������� �������-����������
        _shader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        _shader->setMat4Uniform("projectionMatrix", _camera.projMatrix);

        glm::vec3 lightDir = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta));
        _shader->setVec3Uniform("light.dir", lightDir);
        _shader->setVec3Uniform("light.La", _lightAmbientColor);
        _shader->setVec3Uniform("light.Ld", _lightDiffuseColor);

        //��������� �� ���������� ������� ������ ����� � ��������� ���������
        {
            _shader->setMat4Uniform("modelMatrix", _cube->modelMatrix());
            _shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _cube->modelMatrix()))));

            _shader->setVec3Uniform("material.Ka", glm::vec3(0.0, 1.0, 0.0));
            _shader->setVec3Uniform("material.Kd", glm::vec3(0.0, 1.0, 0.0));

            _cube->draw();
        }

        {
            _shader->setMat4Uniform("modelMatrix", _sphere->modelMatrix());
            _shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _sphere->modelMatrix()))));

            _shader->setVec3Uniform("material.Ka", glm::vec3(1.0, 1.0, 1.0));
            _shader->setVec3Uniform("material.Kd", glm::vec3(1.0, 1.0, 1.0));

            _sphere->draw();
        }

        {
            _shader->setMat4Uniform("modelMatrix", _bunny->modelMatrix());
            _shader->setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * _bunny->modelMatrix()))));

            _shader->setVec3Uniform("material.Ka", glm::vec3(_rabbitAmbientColor));
            _shader->setVec3Uniform("material.Kd", glm::vec3(_rabbitDiffuseColor));

            _bunny->draw();
        }
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}