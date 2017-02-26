#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
��� � ������. ���������� ����������� �������. �������� �������.
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _bunny;

    ShaderProgramPtr _shader;

    void makeScene() override
    {
        Application::makeScene();

        _cameraMover = std::make_shared<FreeCameraMover>();

        //������� ��� � �����
        _cube = makeCube(0.5f);
        _cube->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f)));

        //������� ��� �� �����
        _bunny = loadFromFile("models/bunny.obj");
        _bunny->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

        //������� ��������� ���������        
        _shader = std::make_shared<ShaderProgram>("shaders2/shaderNormal.vert", "shaders2/shader.frag");
    }

    void update() override
    {
        Application::update();

        //������� �������
        float angle = (float)glfwGetTime();

        glm::mat4 mat;
        mat = glm::translate(mat, glm::vec3(0.0f, 0.5f, 0.0));
        mat = glm::rotate(mat, angle, glm::vec3(0.0f, 0.0f, 1.0f));

        _bunny->setModelMatrix(mat);
    }

    void draw() override
    {
        Application::draw();

        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //������������� ������
        _shader->use();

        //������������� ����� �������-����������
        _shader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        _shader->setMat4Uniform("projectionMatrix", _camera.projMatrix);

        //������ ������ ���
        _shader->setMat4Uniform("modelMatrix", _cube->modelMatrix());
        _cube->draw();

        //������ ������ ���
        _shader->setMat4Uniform("modelMatrix", _bunny->modelMatrix());
        _bunny->draw();
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}