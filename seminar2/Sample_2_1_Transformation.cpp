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

    GLuint _shaderProgram;
    GLuint _modelMatrixUniform;
    GLuint _viewMatrixUniform;
    GLuint _projMatrixUniform;

    virtual void makeScene()
    {
        Application::makeScene();

        //������� ��� � ������� ������ ���������������� ������
        _cube = makeCube(0.5f);
        _cube->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f)));

        //������� ��� ���� ��� � ������� ������ ���������������� ������
        _bunny = loadFromFile("models/bunny.obj");
        _bunny->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

        //=========================================================

        ShaderProgram sp;
        sp.createProgram("shaders2/shader.vert", "shaders2/shader.frag");
        _shaderProgram = sp.id();

        _modelMatrixUniform = glGetUniformLocation(_shaderProgram, "modelMatrix");
        _viewMatrixUniform = glGetUniformLocation(_shaderProgram, "viewMatrix");
        _projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");
    }

    virtual void draw()
    {
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(_shaderProgram); //������������� ������

        glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_camera.projMatrix)); //��������� �� ���������� ������� ��������
        glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_camera.viewMatrix)); //��������� �� ���������� ������� ����

        glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_cube->modelMatrix())); //��������� �� ���������� ������� ������ ������� ����
        _cube->draw(); //������ ������ ���

        float angle = (float)glfwGetTime();
        _bunny->setModelMatrix(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0)), angle, glm::vec3(0.0f, 0.0f, 1.0f))); //�������� ������� ������ ������� ����

        glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_bunny->modelMatrix())); //��������� �� ���������� ������� ������ ������� ����
        _bunny->draw(); //������ ������ ���
    }

    //� ���� ������� ��� ���
    void initGUI() override { }
    void updateGUI() override { }
    void drawGUI() override { }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}