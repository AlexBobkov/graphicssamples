#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
Куб и кролик. Управление виртуальной камерой. Вращение кролика.
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

        //Создаем меш с кубом
        _cube = makeCube(0.5f);
        _cube->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f)));

        //Создаем меш из файла
        _bunny = loadFromFile("models/bunny.obj");
        _bunny->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

        //Создаем шейдерную программу        
        _shader = std::make_shared<ShaderProgram>();
        _shader->createProgram("shaders2/shader.vert", "shaders2/shader.frag");
    }

    void draw() override
    {
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _shader->use(); //Устанавливаем шейдер

        _shader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        _shader->setMat4Uniform("projectionMatrix", _camera.projMatrix);

        _shader->setMat4Uniform("modelMatrix", _cube->modelMatrix());
        _cube->draw(); //Рисуем первый меш

        float angle = (float)glfwGetTime();
        _bunny->setModelMatrix(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0)), angle, glm::vec3(0.0f, 0.0f, 1.0f))); //Изменяем матрицу модели второго меша

        _shader->setMat4Uniform("modelMatrix", _bunny->modelMatrix());
        _bunny->draw(); //Рисуем второй меш
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