#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
Четырехугольник в Clip Space. Рисование во фрагментном шейдере. Фрактал Мандельброта.
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
        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Подключаем шейдер
        _shader->use();

        //Рисуем квад
        _quad->draw();
    }

    //В этом примере нет ГУИ
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