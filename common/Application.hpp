#pragma once

#include <Camera.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

class Application
{
public:
    Application();
    ~Application();

    /**
    Запускает приложение
    */
    void start();

    /**
    Обрабатывает нажатия кнопок на клавитуре.
    См. сигнатуру GLFWkeyfun библиотеки GLFW
    */
    virtual void handleKey(int key, int scancode, int action, int mods);

    /**
    Обрабатывает движение мышки
    */
    virtual void handleMouseMove(double xpos, double ypos);

    /**
    Обрабатывает колесико мыши
    */
    virtual void handleScroll(double xoffset, double yoffset);

protected:
    /**
    Инициализирует графический контекст
    */
    virtual void initContext();

    /**
    Настраивает некоторые параметры OpenGL
    */
    virtual void initGL();

    /**
    Инициализирует графический интерфейс пользователя
    */
    virtual void initGUI();

    /**
    Создает трехмерную сцену
    */
    virtual void makeScene();

    /**
    Запускает цикл рендеринга
    */
    void run();

    /**
    Выполняет обновление сцены и виртуальной камеры
    */
    virtual void update();

    /**
    Выполняет обновление графического интерфейса пользователя
    */
    virtual void updateGUI();

    /**
    Отрисовывает один кадр
    */
    virtual void draw();

    /**
    Отрисовывает графический интерфейс пользователя
    */
    virtual void drawGUI();

    //---------------------------------------------

    GLFWwindow* _window = nullptr; //Графичекое окно

    CameraInfo _camera;
    CameraMoverPtr _cameraMover;

    //Время на предыдущем кадре
    double _oldTime = 0.0;
};
