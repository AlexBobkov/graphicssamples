#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
��������� ����� (�������� GL_LINE_STRIP)
*/
class SampleApplication : public Application
{
public:
    MeshPtr _lineStrip;

    GLuint _shaderProgram;
    GLuint _modelMatrixUniform;
    GLuint _viewMatrixUniform;
    GLuint _projMatrixUniform;
    GLuint _timeUniform;

    void makeScene() override
    {
        Application::makeScene();

        unsigned int vertexCount = 36;

        std::vector<float> vertices;
        for (unsigned int i = 0; i < vertexCount; i++)
        {
            vertices.push_back((float)i);
        }

        DataBufferPtr buf0 = std::make_shared<DataBuffer>();
        buf0->setData(vertices.size() * sizeof(float), vertices.data());

        _lineStrip = std::make_shared<Mesh>();
        _lineStrip->setAttribute(0, 1, GL_FLOAT, GL_FALSE, 0, 0, buf0);
        _lineStrip->setPrimitiveType(GL_LINE_STRIP);
        _lineStrip->setVertexCount(vertices.size());

        //=========================================================

        ShaderProgram sp;
        sp.createProgram("shaders3/shaderLine.vert", "shaders3/shader.frag");
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
        glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

        //������ �����
        _lineStrip->draw();
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