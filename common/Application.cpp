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

Application::Application(bool hasGUI) :
_oldTime(0.0),
_phiAng(0.0),
_thetaAng(0.0),
_r(5.0),
_hasGUI(hasGUI),
_oldXPos(0),
_oldYPos(0)
{
}

Application::~Application()
{
    if (_hasGUI)
    {
        ImGui_ImplGlfwGL3_Shutdown();
    }
    glfwTerminate();
}

void Application::start()
{
    initContext();

    initGL();

    if (_hasGUI)
    {
        initGUI();
    }

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

#ifdef USE_CORE_PROFILE
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    int count;
    GLFWmonitor** monitors = glfwGetMonitors(&count);

    if (count == 2)
    {
        //Если 2 монитора, то запускаем приложение в полноэкранном режиме на 2м мониторе                
        _window = glfwCreateWindow(1920, 1080, "MIPT OpenGL demos", monitors[1], NULL);
    }
    else
    {
        _window = glfwCreateWindow(800, 600, "MIPT OpenGL demos", NULL, NULL);
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

        if (_hasGUI)
        {
            updateGUI();
        }

        draw(); //Рисуем один кадр

        if (_hasGUI)
        {
            drawGUI();
        }

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
}

void Application::handleMouseMove(double xpos, double ypos)
{
    if (ImGui::IsMouseHoveringAnyWindow())
    {
        return;
    }

    int state = glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT);
    if (state == GLFW_PRESS)
    {
        double dx = xpos - _oldXPos;
        double dy = ypos - _oldYPos;

        _phiAng -= dx * 0.005;
        _thetaAng += dy * 0.005;
    }

    _oldXPos = xpos;
    _oldYPos = ypos;
}

void Application::handleScroll(double xoffset, double yoffset)
{
    _r += _r * yoffset * 0.05;
}

void Application::update()
{
    double dt = glfwGetTime() - _oldTime;
    _oldTime = glfwGetTime();

    //-----------------------------------------

    double speed = 1.0;
        
    if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
    {
        _phiAng -= speed * dt;
    }
    if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
    {
        _phiAng += speed * dt;
    }
    if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
    {
        _thetaAng += speed * dt;
    }
    if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
    {
        _thetaAng -= speed * dt;
    }
    if (glfwGetKey(_window, GLFW_KEY_R) == GLFW_PRESS)
    {
        _r += _r * dt;
    }
    if (glfwGetKey(_window, GLFW_KEY_F) == GLFW_PRESS)
    {
        _r -= _r * dt;
    }

    //-----------------------------------------

    _thetaAng = glm::clamp(_thetaAng, -glm::pi<double>() * 0.49, glm::pi<double>() * 0.49);

    //Вычисляем положение виртуальной камеры в мировой системе координат по формуле сферических координат
    glm::vec3 pos = glm::vec3(glm::cos(_phiAng) * glm::cos(_thetaAng), glm::sin(_phiAng) * glm::cos(_thetaAng), glm::sin(_thetaAng) + 0.5f) * (float)_r;

    //Обновляем матрицу вида
    _camera.viewMatrix = glm::lookAt(pos, glm::vec3(0.0f, 0.0f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f));

    //-----------------------------------------

    int width, height;
    glfwGetFramebufferSize(_window, &width, &height);

    //Обновляем матрицу проекции на случай, если размеры окна изменились
    _camera.projMatrix = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.f);
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