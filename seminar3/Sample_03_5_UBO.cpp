#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
��������� �������� ��������
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _bunny;

    ShaderProgramPtr _shader;

    GLuint _ubo;

    void makeScene() override
    {
        Application::makeScene();

        _cube = makeCube(0.5);
        _cube->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f)));

        _bunny = loadFromFile("models/bunny.obj");
        _bunny->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

        //=========================================================
        //������������� ��������

        _shader = std::make_shared<ShaderProgram>();
        _shader->createProgram("shaders3/shaderUBO.vert", "shaders3/shader.frag");

        //=========================================================
        //������������� Uniform Buffer Object

        glGenBuffers(1, &_ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(_camera), &_camera, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, _ubo); //0� ����� ��������
    }

    void update() override
    {
        Application::update();

        //��������� ���������� Uniform Buffer Object

        glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
        GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
        memcpy(p, &_camera, sizeof(_camera));
        glUnmapBuffer(GL_UNIFORM_BUFFER);
    }
    
    void draw() override
    {
        Application::draw();

        //�������� ������� ������� ������ � ��������� �������
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //������� ������ ����� � ������� �� ����������� ���������� ����������� �����
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //���������� ������
        _shader->use();

        //��������� �� ���������� �������� �������-����������
        unsigned int blockIndex = glGetUniformBlockIndex(_shader->id(), "shader_data");        
        glUniformBlockBinding(_shader->id(), blockIndex, 0); //0� ����� ��������

        //��������� �� ���������� ������� ������ ����� � ��������� ���������
        _shader->setMat4Uniform("modelMatrix", _cube->modelMatrix());
        _cube->draw();

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