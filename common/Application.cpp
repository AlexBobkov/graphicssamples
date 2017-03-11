#include <Application.hpp>

#include <iostream>
#include <vector>
#include <cstdlib>

//======================================

//Функция обратного вызова для обработки нажатий на клавиатуре
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->handleKey(key, scancode, action, mods);
}

void windowSizeChangedCallback(GLFWwindow* window, int width, int height)
{
}

void mouseButtonPressedCallback(GLFWwindow* window, int button, int action, int mods)
{
}

void mouseCursosPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->handleMouseMove(xpos, ypos);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->handleScroll(xoffset, yoffset);
}

//======================================

Application::Application() :
    _cameraMover(std::make_shared<OrbitCameraMover>())
{
}

Application::~Application()
{
    ImGui_ImplGlfwGL3_Shutdown();
    glfwTerminate();
}

void Application::start()
{
    initContext();

    initGL();

    initGUI();

    makeScene();

    run();
}

void Application::initContext()
{
    if (!glfwInit())
    {
        std::cerr << "ERROR: could not start GLFW3\n";
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int count;
    GLFWmonitor** monitors = glfwGetMonitors(&count);

    if (count == 2)
    {
        //Если 2 монитора, то запускаем приложение в полноэкранном режиме на 2м мониторе                
        _window = glfwCreateWindow(1920, 1080, "MIPT OpenGL demos", monitors[1], nullptr);
    }
    else
    {
        _window = glfwCreateWindow(800, 600, "MIPT OpenGL demos", nullptr, nullptr);
    }
    if (!_window)
    {
        std::cerr << "ERROR: could not open window with GLFW3\n";
        glfwTerminate();
        exit(1);
    }
    glfwMakeContextCurrent(_window);

    glfwSwapInterval(0); //Отключаем вертикальную синхронизацию

    glfwSetWindowUserPointer(_window, this); //Регистрируем указатель на данный объект, чтобы потом использовать его в функциях обратного вызова}

    glfwSetKeyCallback(_window, keyCallback); //Регистрирует функцию обратного вызова для обработки событий клавиатуры
    glfwSetWindowSizeCallback(_window, windowSizeChangedCallback);
    glfwSetMouseButtonCallback(_window, mouseButtonPressedCallback);
    glfwSetCursorPosCallback(_window, mouseCursosPosCallback);
    glfwSetScrollCallback(_window, scrollCallback);
}

void Application::initGL()
{
    glewExperimental = GL_TRUE;
    glewInit();

    const GLubyte* renderer = glGetString(GL_RENDERER); //Получаем имя рендерера
    const GLubyte* version = glGetString(GL_VERSION); //Получаем номер версии
    std::cout << "Renderer: " << renderer << std::endl;
    std::cout << "OpenGL version supported: " << version << std::endl;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void Application::makeScene()
{
    _camera.viewMatrix = glm::lookAt(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    _camera.projMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
}

void Application::run()
{
    while (!glfwWindowShouldClose(_window)) //Пока окно не закрыто
    {
        glfwPollEvents(); //Проверяем события ввода

        update(); //Обновляем сцену и положение виртуальной камеры

        updateGUI();

        draw(); //Рисуем один кадр

        drawGUI();

        glfwSwapBuffers(_window); //Переключаем передний и задний буферы
    }
}

void Application::handleKey(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_ESCAPE)
        {
            glfwSetWindowShouldClose(_window, GL_TRUE);
        }
    }

    _cameraMover->handleKey(_window, key, scancode, action, mods);
}

void Application::handleMouseMove(double xpos, double ypos)
{
    if (ImGui::IsMouseHoveringAnyWindow())
    {
        return;
    }

    _cameraMover->handleMouseMove(_window, xpos, ypos);
}

void Application::handleScroll(double xoffset, double yoffset)
{
    _cameraMover->handleScroll(_window, xoffset, yoffset);
}

void Application::update()
{
    double dt = glfwGetTime() - _oldTime;
    _oldTime = glfwGetTime();

    //-----------------------------------------

    _cameraMover->update(_window, dt);
    _camera = _cameraMover->cameraInfo();
}

void Application::draw()
{
}

//====================================================

void Application::initGUI()
{
    ImGui_ImplGlfwGL3_Init(_window, false);
}

void Application::updateGUI()
{
    ImGui_ImplGlfwGL3_NewFrame();
}

void Application::drawGUI()
{
    ImGui::Render();
}