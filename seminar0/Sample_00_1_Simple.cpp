#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

//Функция обратного вызова для обработки событий клавиатуры
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE)
    {
        //Если нажата клавиша ESCAPE, то закрываем окно
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main()
{
    //Инициализируем библиотеку GLFW
    if (!glfwInit())
    {
        std::cerr << "ERROR: could not start GLFW3\n";
        exit(1);
    }

    //Устанавливаем параметры создания графического контекста
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Создаем графический контекст (окно)
    GLFWwindow* window = glfwCreateWindow(800, 600, "MIPT OpenGL demos", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "ERROR: could not open window with GLFW3\n";
        glfwTerminate();
        exit(1);
    }

    //Делаем этот контекст текущим
    glfwMakeContextCurrent(window);

    //Устанавливаем функцию обратного вызова для обработки событий клавиатуры
    glfwSetKeyCallback(window, keyCallback);

    //Инициализируем библиотеку GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    //=========================================================

    //Координаты вершин треугольника
    float points[] =
    {
        0.0f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f
    };

    //Создаем буфер VertexBufferObject для хранения координат на видеокарте
    GLuint vbo;
    glGenBuffers(1, &vbo);

    //Делаем этот буфер текущим
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    //Копируем содержимое массива в буфер на видеокарте
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

    //=========================================================

    //Создаем объект VertexArrayObject для хранения настроек полигональной модели
    GLuint vao;
    glGenVertexArrays(1, &vao);

    //Делаем этот объект текущим
    glBindVertexArray(vao);

    //Делаем буфер с координатами текущим
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    //Включаем 0й вершинный атрибут
    glEnableVertexAttribArray(0);

    //Устанавливаем настройки:
    //0й атрибут,
    //3 компоненты типа GL_FLOAT,
    //не нужно нормализовать,
    //0 - значения расположены в массиве впритык,
    //0 - сдвиг от начала
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));

    glBindVertexArray(0);

    //=========================================================

    //Вершинный шейдер
    const char* vertexShaderText =
        "#version 330\n"

        "layout(location = 0) in vec3 vertexPosition;\n"

        "void main()\n"
        "{\n"
        "   gl_Position = vec4(vertexPosition, 1.0);\n"
        "}\n";

    //Создаем шейдерный объект
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);

    //Передаем в шейдерный объект текст шейдера
    glShaderSource(vs, 1, &vertexShaderText, nullptr);

    //Компилируем шейдер
    glCompileShader(vs);

    //Проверяем ошибки компиляции
    int status = -1;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        GLint errorLength;
        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &errorLength);
        
        std::vector<char> errorMessage;
        errorMessage.resize(errorLength);

        glGetShaderInfoLog(vs, errorLength, 0, errorMessage.data());

        std::cerr << "Failed to compile the shader:\n" << errorMessage.data() << std::endl;
        
        exit(1);
    }

    //=========================================================

    //Фрагментный шейдер
    const char* fragmentShaderText =
        "#version 330\n"

        "out vec4 fragColor;\n"

        "void main()\n"
        "{\n"
        "    fragColor = vec4(0.5, 0.0, 1.0, 1.0);\n"
        "}\n";

    //Создаем шейдерный объект
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

    //Передаем в шейдерный объект текст шейдера
    glShaderSource(fs, 1, &fragmentShaderText, nullptr);

    //Компилируем шейдер
    glCompileShader(fs);

    //Проверяем ошибки компиляции
    status = -1;
    glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        GLint errorLength;
        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &errorLength);

        std::vector<char> errorMessage;
        errorMessage.resize(errorLength);

        glGetShaderInfoLog(fs, errorLength, 0, errorMessage.data());

        std::cerr << "Failed to compile the shader:\n" << errorMessage.data() << std::endl;

        exit(1);
    }

    //=========================================================

    //Создаем шейдерную программу
    GLuint program = glCreateProgram();

    //Прикрепляем шейдерные объекты
    glAttachShader(program, vs);
    glAttachShader(program, fs);

    //Линкуем программу
    glLinkProgram(program);

    //Проверяем ошибки линковки
    status = -1;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        GLint errorLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &errorLength);

        std::vector<char> errorMessage;
        errorMessage.resize(errorLength);

        glGetProgramInfoLog(program, errorLength, 0, errorMessage.data());

        std::cerr << "Failed to link the program:\n" << errorMessage.data() << std::endl;

        exit(1);
    }

    //=========================================================

    //Цикл рендеринга (пока окно не закрыто)
    while (!glfwWindowShouldClose(window))
    {
        //Проверяем события ввода (здесь вызывается функция обратного вызова для обработки событий клавиатуры)
        glfwPollEvents();

        //Получаем размеры экрана (окна)
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        //Устанавливаем порт вывода на весь экран (окно)
        glViewport(0, 0, width, height);

        //Очищаем порт вывода (буфер цвета и буфер глубины)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Подключаем шейдерную программу
        glUseProgram(program);

        //Подключаем VertexArrayObject с настойками полигональной модели
        glBindVertexArray(vao);

        //Рисуем полигональную модель (состоит из треугольников, сдвиг 0, количество вершин 3)
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window); //Переключаем передний и задний буферы
    }

    //Удаляем созданные объекты OpenGL
    glDeleteProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    glfwTerminate();

    return 0;
}