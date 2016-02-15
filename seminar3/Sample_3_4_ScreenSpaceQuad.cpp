#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
��������������� � Clip Space. ��������� �� ����������� �������. ������� ������������.
*/
class SampleApplication : public Application
{
public:
    MeshPtr _quad;

    ShaderProgramPtr _shader;

    void makeScene() override
    {
        Application::makeScene();

        _quad = makeScreenAlignedQuad();

        //=========================================================

        std::string vertFilename = "shaders3/shaderQuad.vert";
        std::string fragFilename = "shaders3/shaderQuad.frag";

        bool mandelbrot = true;
        if (mandelbrot)
        {
            fragFilename = "shaders3/shaderQuadMandelbrot.frag";
        }

        _shader = std::make_shared<ShaderProgram>();
        _shader->createProgram(vertFilename, fragFilename);
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

        //������ ����
        _quad->draw();
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