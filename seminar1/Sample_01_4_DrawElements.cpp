#include <Application.hpp>

#include <iostream>
#include <vector>

/**
3 грани куба (вариант с индексами)
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

        float points[] =
        {
            //coords
            -0.3f, 0.3f, 0.0f,
            0.3f, 0.3f, 0.0f,
            0.3f, -0.3f, 0.0f,
            -0.3f, -0.3f, 0.0f,
            -0.3f, 0.3f, -1.0f,
            -0.3f, -0.3f, -1.0f,
            0.3f, -0.3f, -1.0f,

            //colors
            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
        };

        int vertexCount = sizeof(points) / sizeof(float) / 7;

        unsigned short indices[] =
        {
            0, 1, 2,
            0, 2, 3,
            4, 0, 3,
            4, 3, 5,
            5, 6, 2,
            5, 2, 3
        };

        int indicesCount = sizeof(indices) / sizeof(unsigned short);

        //Создаем буфер VertexBufferObject для хранения координат на видеокарте
        GLuint vbo;
        glGenBuffers(1, &vbo);

        //Делаем этот буфер текущим
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        //Копируем содержимое массива в буфер на видеокарте
        glBufferData(GL_ARRAY_BUFFER, vertexCount * 7 * sizeof(float), points, GL_STATIC_DRAW);

        //=========================================================

        //Создаем ещё один буфер VertexBufferObject для хранения индексов
        unsigned int ibo;
        glGenBuffers(1, &ibo);

        //Делаем этот буфер текущим
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        //Копируем содержимое массива индексов в буфер на видеокарте
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(unsigned short), indices, GL_STATIC_DRAW);

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

        //Устанавливаем настройки: 0й атрибут, 3 компоненты типа GL_FLOAT, не нужно нормализовать, 0 - значения расположены в массиве впритык, 0 - сдвиг от начала
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        //Устанавливаем настройки: 1й атрибут, 4 компоненты типа GL_FLOAT, не нужно нормализовать, 0 - значения расположены в массиве впритык, 84 - сдвиг от начала массива
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(vertexCount * 4 * 3));

        //Подключаем буфер с индексами
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        glBindVertexArray(0);

        //=========================================================

        //Вершинный шейдер
        const char* vertexShaderText =
            "#version 330\n"

            "uniform mat4 matrix;\n"

            "layout(location = 0) in vec3 vertexPosition;\n"
            "layout(location = 1) in vec4 vertexColor;\n"

            "out vec4 color;\n"

            "void main()\n"
            "{\n"
            "   color = vertexColor;\n"
            "   gl_Position = matrix * vec4(vertexPosition, 1.0);\n"
            "}\n";

        //Создаем шейдерный объект
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);

        //Передаем в шейдерный объект текст шейдера
        glShaderSource(vs, 1, &vertexShaderText, NULL);

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
        glShaderSource(fs, 1, &fragmentShaderText, NULL);

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

        //Задаем матрицу поворота, чтобы смотреть на куб сбоку
        glm::mat4 mat = glm::rotate(glm::mat4(1.0f), 0.2f, glm::vec3(-1.0f, 1.0f, 0.0f));

        //Копируем матрицу на видеокарту в виде юниформ-переменной
        GLint uniformLoc = glGetUniformLocation(_program, "matrix");
        glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mat));

        //Подключаем VertexArrayObject с настойками полигональной модели
        glBindVertexArray(_vao);

        //Рисуем индексную полигональную модель (3 грани куба состоят из 6 треугольников, количество индексов 18, тип данных индекса unsigned short, сдвиг 0)
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0); //Рисуем с помощью индексов
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}