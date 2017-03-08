#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
Несколько примеров шейдеров
*/
class SampleApplication : public Application
{
public:
    MeshPtr _cube;
    MeshPtr _bunny;

    std::vector<ShaderProgramPtr> _shaders;
    int _currentIndex = 0;

    void makeScene() override
    {
        Application::makeScene();

        _cube = makeCube(0.5);
        _cube->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f)));

        _bunny = loadFromFile("models/bunny.obj");
        _bunny->setModelMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

        //=========================================================

        _shaders.push_back(std::make_shared<ShaderProgram>("shaders2/simple.vert", "shaders2/simple.frag"));
        _shaders.push_back(std::make_shared<ShaderProgram>("shaders2/simpleMat.vert", "shaders2/simple.frag"));
        _shaders.push_back(std::make_shared<ShaderProgram>("shaders2/shader.vert", "shaders2/shader.frag"));
        _shaders.push_back(std::make_shared<ShaderProgram>("shaders2/shaderTime.vert", "shaders2/shader.frag"));
        _shaders.push_back(std::make_shared<ShaderProgram>("shaders2/shader.vert", "shaders2/shaderTime.frag"));
        _shaders.push_back(std::make_shared<ShaderProgram>("shaders2/shaderTimeCoord.vert", "shaders2/shaderTimeCoord.frag"));
        _shaders.push_back(std::make_shared<ShaderProgram>("shaders2/shader.vert", "shaders2/shaderDiscard.frag"));
    }

    void updateGUI() override
    {
        Application::updateGUI();

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
        if (ImGui::Begin("MIPT OpenGL Sample", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("FPS %.1f", ImGui::GetIO().Framerate);

            ImGui::RadioButton("no matrix", &_currentIndex, 0);
            ImGui::RadioButton("matrix", &_currentIndex, 1);
            ImGui::RadioButton("colored", &_currentIndex, 2);
            ImGui::RadioButton("pos animation", &_currentIndex, 3);
            ImGui::RadioButton("color animation", &_currentIndex, 4);
            ImGui::RadioButton("color from pos", &_currentIndex, 5);
            ImGui::RadioButton("discard", &_currentIndex, 6);
        }
        ImGui::End();
    }

    void draw() override
    {
        Application::draw();

        //Получаем текущие размеры экрана и выставлям вьюпорт
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        glViewport(0, 0, width, height);

        //Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Подключаем шейдер
        _shaders[_currentIndex]->use();

        //Загружаем на видеокарту значения юниформ-переменные: время и матрицы
        _shaders[_currentIndex]->setFloatUniform("time", (float)glfwGetTime()); //передаем время в шейдер

        _shaders[_currentIndex]->setMat4Uniform("viewMatrix", _camera.viewMatrix);
        _shaders[_currentIndex]->setMat4Uniform("projectionMatrix", _camera.projMatrix);

        //Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
        _shaders[_currentIndex]->setMat4Uniform("modelMatrix", _cube->modelMatrix());
        _cube->draw();

        _shaders[_currentIndex]->setMat4Uniform("modelMatrix", _bunny->modelMatrix());
        _bunny->draw();
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}