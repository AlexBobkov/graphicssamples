#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
��������� ����� (�������� GL_POINTS)
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
    GLuint _timeUniform;

    void makeScene() override
    {
        Application::makeScene();

        //�������� ����������� �����
        glEnable(GL_POINT_SMOOTH);
        //�������� ��������� ������� ����� ����� ������
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

        _cube = makeCube(0.5);
        _cube->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f)));
        _cube->setPrimitiveType(GL_POINTS);

        _bunny = loadFromFile("models/bunny.obj");
        _bunny->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
        _bunny->setPrimitiveType(GL_POINTS);

        //=========================================================

        ShaderProgram sp;
        sp.createProgram("shaders3/shaderPoint.vert", "shaders3/shader.frag");
        _shaderProgram = sp.id();

        _modelMatrixUniform = glGetUniformLocation(_shaderProgram, "modelMatrix");
        _viewMatrixUniform = glGetUniformLocation(_shaderProgram, "viewMatrix");
        _projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");
        _timeUniform = glGetUniformLocation(_shaderProgram, "time");
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
        glUseProgram(_shaderProgram);

        //��������� �� ���������� �������� �������-����������: ����� � �������
        glUniform1f(_timeUniform, (float)glfwGetTime()); //�������� ����� � ������	

        glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_camera.projMatrix));
        glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_camera.viewMatrix));

        //��������� �� ���������� ������� ������ ����� � ��������� ���������
        glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_cube->modelMatrix()));
        _cube->draw();

        glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_bunny->modelMatrix()));
        _bunny->draw();
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