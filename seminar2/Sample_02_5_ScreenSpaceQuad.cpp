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

    std::vector<ShaderProgramPtr> _shaders;
    int _currentIndex = 0;

    void makeScene() override
    {
        Application::makeScene();

        _quad = makeScreenAlignedQuad();

        _shaders.push_back(std::make_shared<ShaderProgram>("shaders2/shaderQuad.vert", "shaders2/shaderQuad.frag"));
        _shaders.push_back(std::make_shared<ShaderProgram>("shaders2/shaderQuad.vert", "shaders2/shaderQuadMandelbrot.frag"));
    }

    void updateGUI() override
    {
        Application::updateGUI();

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
        if (ImGui::Begin("MIPT OpenGL Sample", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("FPS %.1f", ImGui::GetIO().Framerate);

            ImGui::RadioButton("circle", &_currentIndex, 0);
            ImGui::RadioButton("mandelbrot", &_currentIndex, 1);
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

        //���������� ������
        _shaders[_currentIndex]->use();

        //������ ����
        _quad->draw();
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}