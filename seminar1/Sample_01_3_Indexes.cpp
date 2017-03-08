#include <Application.hpp>

#include <iostream>
#include <vector>

/**
3 грани куба (вариант без индексов)
*/
class SampleApplication : public Application
{
public:
    //Идентификатор VertexArrayObject, который хранит настройки полигональной модели
    GLuint _noIndexedModed;
    GLsizei _noIndexedModelVertexCount = 0;

    GLuint _indexedModed;
    GLsizei _indexedModelVertexCount = 0;
    GLsizei _indexCount = 0;

    //Идентификатор шейдерной программы
    GLuint _program;

    enum ModelType
    {
        NoIndexedModel,
        IndexedModel
    };

    int _currentModel = 0;

    void makeModelWithoutIndexes()
    {
        //Координаты и цвета вершин
        float points[] =
        {
            -0.5f, -0.5f, 0.0f,      //координаты
            1.0f, 0.0f, 0.0f, 1.0f,  //цвет

            0.5f, 0.5f, 0.0f,
            1.0f, 0.0f, 0.0f, 1.0f,

            0.5f, -0.5f, 0.0f,
            1.0f, 0.0f, 0.0f, 1.0f,

            -0.5f, -0.5f, 0.0f,
            0.0f, 1.0f, 0.0f, 1.0f,

            -0.5f, 0.5f, 0.0f,
            0.0f, 1.0f, 0.0f, 1.0f,

            0.5f, 0.5f, 0.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
        };

        _noIndexedModelVertexCount = sizeof(points) / sizeof(float) / 7;

        //Создаем буфер VertexBufferObject для хранения координат на видеокарте
        GLuint vbo;
        glGenBuffers(1, &vbo);

        //Делаем этот буфер текущим
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        //Копируем содержимое массива в буфер на видеокарте
        glBufferData(GL_ARRAY_BUFFER, _noIndexedModelVertexCount * 7 * sizeof(float), points, GL_STATIC_DRAW);

        //=========================================================

        //Создаем объект VertexArrayObject для хранения настроек полигональной модели
        glGenVertexArrays(1, &_noIndexedModed);

        //Делаем этот объект текущим
        glBindVertexArray(_noIndexedModed);

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
        //28 - расстояние в байтах между 2мя соседними значениями,
        //0 - сдвиг в байтах от начала
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);

        //Устанавливаем настройки:
        //1й атрибут,
        //4 компоненты типа GL_FLOAT,
        //не нужно нормализовать,
        //28 - расстояние в байтах между 2мя соседними значениями,
        //12 - сдвиг в байтах от начала массива
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

        glBindVertexArray(0);
    }

    void makeModelWithIndexes()
    {
        float points[] =
        {
            -0.5f, 0.5f, 0.0f,        //координаты
            1.0f, 0.0f, 0.0f, 1.0f,   //цвет

            0.5f, 0.5f, 0.0f,
            0.0f, 1.0f, 0.0f, 1.0f,

            0.5f, -0.5f, 0.0f,
            0.0f, 0.0f, 1.0f, 1.0f,

            -0.5f, -0.5f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f,
        };

        _indexedModelVertexCount = sizeof(points) / sizeof(float) / 7;

        unsigned short indices[] =
        {
            3, 1, 2,
            3, 0, 1,
        };

        _indexCount = sizeof(indices) / sizeof(unsigned short);

        //Создаем буфер VertexBufferObject для хранения координат на видеокарте
        GLuint vbo;
        glGenBuffers(1, &vbo);

        //Делаем этот буфер текущим
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        //Копируем содержимое массива в буфер на видеокарте
        glBufferData(GL_ARRAY_BUFFER, _indexedModelVertexCount * 7 * sizeof(float), points, GL_STATIC_DRAW);

        //=========================================================

        //Создаем ещё один буфер VertexBufferObject для хранения индексов
        unsigned int ibo;
        glGenBuffers(1, &ibo);

        //Делаем этот буфер текущим
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        //Копируем содержимое массива индексов в буфер на видеокарте
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexCount * sizeof(unsigned short), indices, GL_STATIC_DRAW);

        //=========================================================

        //Создаем объект VertexArrayObject для хранения настроек полигональной модели
        glGenVertexArrays(1, &_indexedModed);

        //Делаем этот объект текущим
        glBindVertexArray(_indexedModed);

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
        //28 - значения расположены в массиве впритык,
        //0 - сдвиг от начала
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);

        //Устанавливаем настройки:
        //1й атрибут,
        //4 компоненты типа GL_FLOAT,
        //не нужно нормализовать,
        //28 - значения расположены в массиве впритык,
        //12 - сдвиг от начала массива
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

        //Подключаем буфер с индексами
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        glBindVertexArray(0);
    }

    void makeScene() override
    {
        Application::makeScene();

        makeModelWithoutIndexes();
        makeModelWithIndexes();

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

    void updateGUI() override
    {
        Application::updateGUI();

        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
        if (ImGui::Begin("MIPT OpenGL Sample", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("FPS %.1f", ImGui::GetIO().Framerate);

            ImGui::RadioButton("no indices", &_currentModel, ModelType::NoIndexedModel);
            ImGui::RadioButton("index", &_currentModel, ModelType::IndexedModel);
        }
        ImGui::End();
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

        if (_currentModel == ModelType::NoIndexedModel)
        {
            //Подключаем VertexArrayObject с настойками полигональной модели
            glBindVertexArray(_noIndexedModed);

            //Рисуем полигональную модель без индексов
            glDrawArrays(GL_TRIANGLES, 0, _noIndexedModelVertexCount);
        }
        else
        {
            //Подключаем VertexArrayObject с настойками полигональной модели        
            glBindVertexArray(_indexedModed);

            //Рисуем индексную полигональную модель
            glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_SHORT, 0); //Рисуем с помощью индексов
        }
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}