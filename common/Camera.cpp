#include <Camera.hpp>

#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>

void OrbitCameraMover::handleKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

void OrbitCameraMover::handleMouseMove(GLFWwindow* window, double xpos, double ypos)
{
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS)
    {
        double dx = xpos - _oldXPos;
        double dy = ypos - _oldYPos;

        _phiAng -= dx * 0.005;
        _thetaAng += dy * 0.005;

        _thetaAng = glm::clamp(_thetaAng, -glm::pi<double>() * 0.49, glm::pi<double>() * 0.49);
    }

    _oldXPos = xpos;
    _oldYPos = ypos;
}

void OrbitCameraMover::handleScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    _r += _r * yoffset * 0.05;
}

void OrbitCameraMover::update(GLFWwindow* window, double dt)
{
    double speed = 1.0;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        _phiAng -= speed * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        _phiAng += speed * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        _thetaAng += speed * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        _thetaAng -= speed * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        _r += _r * dt;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        _r -= _r * dt;
    }

    _thetaAng = glm::clamp(_thetaAng, -glm::pi<double>() * 0.49, glm::pi<double>() * 0.49);

    //-----------------------------------------

    //Вычисляем положение виртуальной камеры в мировой системе координат по формуле сферических координат
    glm::vec3 pos = glm::vec3(glm::cos(_phiAng) * glm::cos(_thetaAng), glm::sin(_phiAng) * glm::cos(_thetaAng), glm::sin(_thetaAng) + 0.5f) * (float)_r;

    //Обновляем матрицу вида
    _camera.viewMatrix = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f));

    //-----------------------------------------

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    //Обновляем матрицу проекции на случай, если размеры окна изменились
    _camera.projMatrix = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.f);
}

//=============================================

FreeCameraMover::FreeCameraMover() :
CameraMover(),
_pos(5.0f, 0.0f, 2.5f)
{       
    //Нам нужно как-нибудь посчитать начальную ориентацию камеры
    _rot = glm::toQuat(glm::lookAt(_pos, glm::vec3(0.0f, 0.0f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f)));
}

void FreeCameraMover::handleKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

void FreeCameraMover::handleMouseMove(GLFWwindow* window, double xpos, double ypos)
{
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS)
    {
        double dx = xpos - _oldXPos;
        double dy = ypos - _oldYPos;

        //Добавляем небольшой поворот вверх/вниз        
        glm::vec3 rightDir = glm::vec3(1.0f, 0.0f, 0.0f) * _rot;
        _rot *= glm::angleAxis(static_cast<float>(dy * 0.005), rightDir);

        //Добавляем небольшой поворов вокруг вертикальной оси
        glm::vec3 upDir(0.0f, 0.0f, 1.0f);
        _rot *= glm::angleAxis(static_cast<float>(dx * 0.005), upDir);
    }

    _oldXPos = xpos;
    _oldYPos = ypos;
}

void FreeCameraMover::handleScroll(GLFWwindow* window, double xoffset, double yoffset)
{
}

void FreeCameraMover::update(GLFWwindow* window, double dt)
{
    float speed = 1.0f;

    //Получаем текущее направление "вперед" в мировой системе координат
    glm::vec3 forwDir = glm::vec3(0.0f, 0.0f, -1.0f) * _rot;

    //Получаем текущее направление "вправо" в мировой системе координат
    glm::vec3 rightDir = glm::vec3(1.0f, 0.0f, 0.0f) * _rot;
        
    //Двигаем камеру вперед/назад
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        _pos += forwDir * speed * static_cast<float>(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        _pos -= forwDir * speed * static_cast<float>(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        _pos -= rightDir * speed * static_cast<float>(dt);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        _pos += rightDir * speed * static_cast<float>(dt);
    }

    //-----------------------------------------

    //Соединяем перемещение и поворот вместе
    _camera.viewMatrix = glm::toMat4(-_rot) * glm::translate(-_pos);
    
    //-----------------------------------------

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    //Обновляем матрицу проекции на случай, если размеры окна изменились
    _camera.projMatrix = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.f);
}