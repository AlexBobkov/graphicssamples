#include <Application.hpp>

#include <iostream>
#include <vector>

/**
3 ����� ���� (������� ��� ��������)
*/
class SampleApplication : public Application
{
public:
    //������������� VertexArrayObject, ������� ������ ��������� ������������� ������
    GLuint _noIndexedModed;
    GLsizei _noIndexedModelVertexCount = 0;

    GLuint _indexedModed;
    GLsizei _indexedModelVertexCount = 0;
    GLsizei _indexCount = 0;

    //������������� ��������� ���������
    GLuint _program;

    enum ModelType
    {
        NoIndexedModel,
        IndexedModel
    };

    int _currentModel = 0;

    void makeModelWithoutIndexes()
    {
        //���������� � ����� ������
        float points[] =
        {
            -0.5f, -0.5f, 0.0f,      //����������
            1.0f, 0.0f, 0.0f, 1.0f,  //����

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

        //������� ����� VertexBufferObject ��� �������� ��������� �� ����������
        GLuint vbo;
        glGenBuffers(1, &vbo);

        //������ ���� ����� �������
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        //�������� ���������� ������� � ����� �� ����������
        glBufferData(GL_ARRAY_BUFFER, _noIndexedModelVertexCount * 7 * sizeof(float), points, GL_STATIC_DRAW);

        //=========================================================

        //������� ������ VertexArrayObject ��� �������� �������� ������������� ������
        glGenVertexArrays(1, &_noIndexedModed);

        //������ ���� ������ �������
        glBindVertexArray(_noIndexedModed);

        //������ ����� � ������������ �������
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        //�������� 0� ��������� ������� - ����������
        glEnableVertexAttribArray(0);

        //�������� 1� ��������� ������� - �����
        glEnableVertexAttribArray(1);

        //������������� ���������:
        //0� �������,
        //3 ���������� ���� GL_FLOAT,
        //�� ����� �������������,
        //28 - ���������� � ������ ����� 2�� ��������� ����������,
        //0 - ����� � ������ �� ������
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);

        //������������� ���������:
        //1� �������,
        //4 ���������� ���� GL_FLOAT,
        //�� ����� �������������,
        //28 - ���������� � ������ ����� 2�� ��������� ����������,
        //12 - ����� � ������ �� ������ �������
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

        glBindVertexArray(0);
    }

    void makeModelWithIndexes()
    {
        float points[] =
        {
            -0.5f, 0.5f, 0.0f,        //����������
            1.0f, 0.0f, 0.0f, 1.0f,   //����

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

        //������� ����� VertexBufferObject ��� �������� ��������� �� ����������
        GLuint vbo;
        glGenBuffers(1, &vbo);

        //������ ���� ����� �������
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        //�������� ���������� ������� � ����� �� ����������
        glBufferData(GL_ARRAY_BUFFER, _indexedModelVertexCount * 7 * sizeof(float), points, GL_STATIC_DRAW);

        //=========================================================

        //������� ��� ���� ����� VertexBufferObject ��� �������� ��������
        unsigned int ibo;
        glGenBuffers(1, &ibo);

        //������ ���� ����� �������
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        //�������� ���������� ������� �������� � ����� �� ����������
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexCount * sizeof(unsigned short), indices, GL_STATIC_DRAW);

        //=========================================================

        //������� ������ VertexArrayObject ��� �������� �������� ������������� ������
        glGenVertexArrays(1, &_indexedModed);

        //������ ���� ������ �������
        glBindVertexArray(_indexedModed);

        //������ ����� � ������������ �������
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        //�������� 0� ��������� ������� - ����������
        glEnableVertexAttribArray(0);

        //�������� 1� ��������� ������� - �����
        glEnableVertexAttribArray(1);

        //������������� ���������:
        //0� �������,
        //3 ���������� ���� GL_FLOAT,
        //�� ����� �������������,
        //28 - �������� ����������� � ������� �������,
        //0 - ����� �� ������
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);

        //������������� ���������:
        //1� �������,
        //4 ���������� ���� GL_FLOAT,
        //�� ����� �������������,
        //28 - �������� ����������� � ������� �������,
        //12 - ����� �� ������ �������
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));

        //���������� ����� � ���������
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        glBindVertexArray(0);
    }

    void makeScene() override
    {
        Application::makeScene();

        makeModelWithoutIndexes();
        makeModelWithIndexes();

        //=========================================================

        //��������� ������
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

        //������� ��������� ������
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);

        //�������� � ��������� ������ ����� �������
        glShaderSource(vs, 1, &vertexShaderText, nullptr);

        //����������� ������
        glCompileShader(vs);

        //��������� ������ ����������
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

        //����������� ������
        const char* fragmentShaderText =
            "#version 330\n"

            "in vec4 color;\n"

            "out vec4 fragColor;\n"

            "void main()\n"
            "{\n"
            "    fragColor = color;\n"
            "}\n";

        //������� ��������� ������
        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

        //�������� � ��������� ������ ����� �������
        glShaderSource(fs, 1, &fragmentShaderText, nullptr);

        //����������� ������
        glCompileShader(fs);

        //��������� ������ ����������
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

        //������� ��������� ���������
        _program = glCreateProgram();

        //����������� ��������� �������
        glAttachShader(_program, vs);
        glAttachShader(_program, fs);

        //������� ���������
        glLinkProgram(_program);

        //��������� ������ ��������
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

        //�������� ������� ������ (����)
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);

        //������������� ���� ������ �� ���� ����� (����)
        glViewport(0, 0, width, height);

        //������� ���� ������ (����� ����� � ����� �������)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //���������� ��������� ���������
        glUseProgram(_program);

        if (_currentModel == ModelType::NoIndexedModel)
        {
            //���������� VertexArrayObject � ���������� ������������� ������
            glBindVertexArray(_noIndexedModed);

            //������ ������������� ������ ��� ��������
            glDrawArrays(GL_TRIANGLES, 0, _noIndexedModelVertexCount);
        }
        else
        {
            //���������� VertexArrayObject � ���������� ������������� ������        
            glBindVertexArray(_indexedModed);

            //������ ��������� ������������� ������
            glDrawElements(GL_TRIANGLES, _indexCount, GL_UNSIGNED_SHORT, 0); //������ � ������� ��������
        }
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}