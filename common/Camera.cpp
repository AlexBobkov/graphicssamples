#include <Camera.hpp>

OrbitCameraMover::OrbitCameraMover():
CameraMover(),
_phiAng(0.0),
_thetaAng(0.0),
_r(5.0),
_oldXPos(0),
_oldYPos(0)
{

}

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