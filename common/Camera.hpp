#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <memory>

struct CameraInfo
{
    glm::mat4 viewMatrix;
    glm::mat4 projMatrix;
};

/**
Класс для управления виртуальной камерой
*/
class CameraMover
{
public:
    CameraMover() {}
    virtual ~CameraMover() {}

    /**
    Возвращает параметры виртуальной камеры в виде 2х матриц: матрицы вида и проекции
    */
    CameraInfo cameraInfo() const { return _camera; }

    /**
    Обрабатывает нажатия кнопок на клавитуре.
    См. сигнатуру GLFWkeyfun библиотеки GLFW
    */
    virtual void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;

    /**
    Обрабатывает движение мышки
    */
    virtual void handleMouseMove(GLFWwindow* window, double xpos, double ypos) = 0;

    /**
    Обрабатывает колесико мыши
    */
    virtual void handleScroll(GLFWwindow* window, double xoffset, double yoffset) = 0;

    /**
    Обновляет положение виртуальной камеры
    */
    virtual void update(GLFWwindow* window, double dt) = 0;

protected:
    CameraInfo _camera;
};

using CameraMoverPtr = std::shared_ptr<CameraMover>;

/**
Положение камеры задается в сферических координатах. Камера всегда смотрит в центр сферы.
*/
class OrbitCameraMover : public CameraMover
{
public:
    void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods) override;
    void handleMouseMove(GLFWwindow* window, double xpos, double ypos) override;
    void handleScroll(GLFWwindow* window, double xoffset, double yoffset) override;
    void update(GLFWwindow* window, double dt) override;

protected:
    //Положение виртуальный камеры задается в сферических координат
    double _phiAng = 0.0;
    double _thetaAng = 0.0;
    double _r = 5.0;

    //Положение курсора мыши на предыдущем кадре
    double _oldXPos = 0.0;
    double _oldYPos = 0.0;
};

/**
Камера всегда летит вперед
*/
class FreeCameraMover : public CameraMover
{
public:
    FreeCameraMover();

    void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods) override;
    void handleMouseMove(GLFWwindow* window, double xpos, double ypos) override;
    void handleScroll(GLFWwindow* window, double xoffset, double yoffset) override;
    void update(GLFWwindow* window, double dt) override;

protected:
    //Положение виртуальный камеры задается в сферических координат
    glm::vec3 _pos;
    glm::quat _rot;

    //Положение курсора мыши на предыдущем кадре
    double _oldXPos = 0.0;
    double _oldYPos = 0.0;
};

