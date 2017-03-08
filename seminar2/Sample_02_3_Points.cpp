#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
Рендеринг точек (примитив GL_POINTS)
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

        //Включает сглаживание точек
        glEnable(GL_POINT_SMOOTH);
        //Включает изменение размера точек через шейдер
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

        _cube = makeCube(0.5);
        _cube->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f)));
        _cube->setPrimitiveType(GL_POINTS);

        _bunny = loadFromFile("models/bunny.obj");
        _bunny->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
        _bunny->setPrimitiveType(GL_POINTS);

        //=========================================================

        _shader = std::make_shared<ShaderProgram>("shaders2/shaderPoint.vert", "shaders2/shader.frag");
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

        //Загружаем на видеокарту значения юниформ-переменные: время и матрицы
        _shader->setFloatUniform("time", (float)glfwGetTime()); //передаем время в шейдер

        _shader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        _shader->setMat4Uniform("projectionMatrix", _camera.projMatrix);

        //Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
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