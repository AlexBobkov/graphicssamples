#include <Application.hpp>

#include <iostream>
#include <vector>

/**
Треугольник с интерполированными цветами
*/
class SampleApplication : public Application
{
public:
    //Идентификатор VertexArrayObject, который хранит настройки полигональной модели
    GLuint _vao;

    //Идентификатор шейдерной программы
    GLuint _program;

    void makeScene() override
    {
        Application::makeScene();

        //Координаты вершин треугольника и далее цвета вершин
        float points[] =
        {
            0.0f, 0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,
        };

        //Создаем буфер VertexBufferObject для хранения координат на видеокарте
        GLuint vbo;
        glGenBuffers(1, &vbo);

        //Делаем этот буфер текущим
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        //Копируем содержимое массива в буфер на видеокарте
        glBufferData(GL_ARRAY_BUFFER, 21 * sizeof(float), points, GL_STATIC_DRAW);

        //=========================================================

        //Создаем объект VertexArrayObject для хранения настроек полигональной модели
        glGenVertexArrays(1, &_vao);

        //Делаем этот объект текущим
        glBindVertexArray(_vao);

        //Делаем буфер с координатами текущим
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        //Включаем 0й вершинный атрибут - координаты
        glEnableVertexAttribArray(0);

        //Включаем 1й вершинный атрибут - цвета
        glEnableVertexAttribArray(1);

        //Устанавливаем настройки:
        //0й атрибут,
        //3 компоненты типа GL_FLOAT,
        //не нужно нормализовать,
        //0 - значения расположены в массиве впритык,
        //0 - сдвиг от начала
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));

        //Устанавливаем настройки:
        //1й атрибут,
        //4 компоненты типа GL_FLOAT,
        //не нужно нормализовать,
        //0 - значения расположены в массиве впритык,
        //36 - сдвиг от начала массива
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(36));

        glBindVertexArray(0);

        //=========================================================

        //Вершинный шейдер
        const char* vertexShaderText =
            "#version 330\n"

            "layout(location = 0) in vec3 vertexPosition;\n"
            "layout(location = 1) in vec4 vertexColor;\n"

            "out vec4 color;\n"

            "void main()\n"
            "{\n"
            "   color = vertexColor;\n"
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

            "in vec4 color;\n"

            "out vec4 fragColor;\n"

            "void main()\n"
            "{\n"
            "    fragColor = color;\n"
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
        _program = glCreateProgram();

        //Прикрепляем шейдерные объекты
        glAttachShader(_program, vs);
        glAttachShader(_program, fs);

        //Линкуем программу
        glLinkProgram(_program);

        //Проверяем ошибки линковки
        status = -1;
        glGetProgramiv(_program, GL_LINK_STATUS, &status);
        if (status != GL_TRUE)
        {
            GLint errorLength;
            glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &errorLength);

            std::vector<char> errorMessage;
            errorMessage.resize(errorLength);

            glGetProgramInfoLog(_program, errorLength, 0, errorMessage.data());

            std::cerr << "Failed to link the program:\n" << errorMessage.data() << std::endl;

            exit(1);
        }
    }

    void draw() override
    {
        Application::draw();

        //Получаем размеры экрана (окна)
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        //Устанавливаем порт вывода на весь экран (окно)
        glViewport(0, 0, width, height);

        //Очищаем порт вывода (буфер цвета и буфер глубины)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Подключаем шейдерную программу
        glUseProgram(_program);

        //Подключаем VertexArrayObject с настойками полигональной модели
        glBindVertexArray(_vao);

        //Рисуем полигональную модель (состоит из треугольников, сдвиг 0, количество вершин 3)
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}