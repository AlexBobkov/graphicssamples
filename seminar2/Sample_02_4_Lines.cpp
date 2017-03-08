#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
Рендеринг линии (примитив GL_LINE_STRIP)
*/
class SampleApplication : public Application
{
public:
    MeshPtr _lineStrip;

    ShaderProgramPtr _shader;

    void makeScene() override
    {
        Application::makeScene();

        unsigned int vertexCount = 36;

        std::vector<float> vertices;
        for (unsigned int i = 0; i < vertexCount; i++)
        {
            vertices.push_back((float)i);
        }

        DataBufferPtr buf0 = std::make_shared<DataBuffer>(GL_ARRAY_BUFFER);
        buf0->setData(vertices.size() * sizeof(float), vertices.data());

        _lineStrip = std::make_shared<Mesh>();
        _lineStrip->setAttribute(0, 1, GL_FLOAT, GL_FALSE, 0, 0, buf0);
        _lineStrip->setPrimitiveType(GL_LINE_STRIP);
        _lineStrip->setVertexCount(vertices.size());

        //=========================================================

        _shader = std::make_shared<ShaderProgram>("shaders2/shaderLine.vert", "shaders2/shader.frag");
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
        _shader->setFloatUniform("time", static_cast<float>(glfwGetTime())); //передаем время в шейдер

        _shader->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        _shader->setMat4Uniform("projectionMatrix", _camera.projMatrix);

        //Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
        _shader->setMat4Uniform("modelMatrix", glm::mat4(1.0f));

        //Рисуем линию
        _lineStrip->draw();
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}