#include <Application.hpp>

#include <iostream>
#include <vector>

/**
3 ����� ���� (������� � ���������)
*/
class SampleApplication : public Application
{
public:
    //������������� VertexArrayObject, ������� ������ ��������� ������������� ������
    GLuint _vao;

    //������������� ��������� ���������
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

        //������� ����� VertexBufferObject ��� �������� ��������� �� ����������
        GLuint vbo;
        glGenBuffers(1, &vbo);

        //������ ���� ����� �������
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        //�������� ���������� ������� � ����� �� ����������
        glBufferData(GL_ARRAY_BUFFER, vertexCount * 7 * sizeof(float), points, GL_STATIC_DRAW);

        //=========================================================

        //������� ��� ���� ����� VertexBufferObject ��� �������� ��������
        unsigned int ibo;
        glGenBuffers(1, &ibo);

        //������ ���� ����� �������
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        //�������� ���������� ������� �������� � ����� �� ����������
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(unsigned short), indices, GL_STATIC_DRAW);

        //=========================================================

        //������� ������ VertexArrayObject ��� �������� �������� ������������� ������
        glGenVertexArrays(1, &_vao);

        //������ ���� ������ �������
        glBindVertexArray(_vao);

        //������ ����� � ������������ �������
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        //�������� 0� ��������� ������� - ����������
        glEnableVertexAttribArray(0);

        //�������� 1� ��������� ������� - �����
        glEnableVertexAttribArray(1);

        //������������� ���������: 0� �������, 3 ���������� ���� GL_FLOAT, �� ����� �������������, 0 - �������� ����������� � ������� �������, 0 - ����� �� ������
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        //������������� ���������: 1� �������, 4 ���������� ���� GL_FLOAT, �� ����� �������������, 0 - �������� ����������� � ������� �������, 84 - ����� �� ������ �������
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(vertexCount * 4 * 3));

        //���������� ����� � ���������
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        glBindVertexArray(0);

        //=========================================================

        //��������� ������
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

        //������� ��������� ������
        GLuint vs = glCreateShader(GL_VERTEX_SHADER);

        //�������� � ��������� ������ ����� �������
        glShaderSource(vs, 1, &vertexShaderText, NULL);

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
        glShaderSource(fs, 1, &fragmentShaderText, NULL);

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

        //������ ������� ��������, ����� �������� �� ��� �����
        glm::mat4 mat = glm::rotate(glm::mat4(1.0f), 0.2f, glm::vec3(-1.0f, 1.0f, 0.0f));

        //�������� ������� �� ���������� � ���� �������-����������
        GLint uniformLoc = glGetUniformLocation(_program, "matrix");
        glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mat));

        //���������� VertexArrayObject � ���������� ������������� ������
        glBindVertexArray(_vao);

        //������ ��������� ������������� ������ (3 ����� ���� ������� �� 6 �������������, ���������� �������� 18, ��� ������ ������� unsigned short, ����� 0)
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0); //������ � ������� ��������
    }
};

int main()
{
    SampleApplication app;
    app.start();

    return 0;
}