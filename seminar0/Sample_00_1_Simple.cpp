#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

//������� ��������� ������ ��� ��������� ������� ����������
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE)
    {
        //���� ������ ������� ESCAPE, �� ��������� ����
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main()
{
    //�������������� ���������� GLFW
    if (!glfwInit())
    {
        std::cerr << "ERROR: could not start GLFW3\n";
        exit(1);
    }

    //������������� ��������� �������� ������������ ���������
#ifdef USE_CORE_PROFILE
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    //������� ����������� �������� (����)
    GLFWwindow* window = glfwCreateWindow(800, 600, "MIPT OpenGL demos", NULL, NULL);
    if (!window)
    {
        std::cerr << "ERROR: could not open window with GLFW3\n";
        glfwTerminate();
        exit(1);
    }

    //������ ���� �������� �������
    glfwMakeContextCurrent(window);

    //������������� ������� ��������� ������ ��� ��������� ������� ����������
    glfwSetKeyCallback(window, keyCallback);

    //�������������� ���������� GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    //=========================================================

    //���������� ������ ������������
    float points[] =
    {
        0.0f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f
    };

    //������� ����� VertexBufferObject ��� �������� ��������� �� ����������
    GLuint vbo;
    glGenBuffers(1, &vbo);

    //������ ���� ����� �������
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    //�������� ���������� ������� � ����� �� ����������
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

    //=========================================================

    //������� ������ VertexArrayObject ��� �������� �������� ������������� ������
    GLuint vao;
    glGenVertexArrays(1, &vao);

    //������ ���� ������ �������
    glBindVertexArray(vao);

    //������ ����� � ������������ �������
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    //�������� 0� ��������� �������
    glEnableVertexAttribArray(0);

    //������������� ���������: 0� �������, 3 ���������� ���� GL_FLOAT, �� ����� �������������, 0 - �������� ����������� � ������� �������, 0 - ����� �� ������
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));

    //=========================================================

    //��������� ������
    const char* vertexShaderText =
        "#version 330\n"

        "layout(location = 0) in vec3 vertexPosition;\n"

        "void main()\n"
        "{\n"
        "   gl_Position = vec4(vertexPosition, 1.0);\n"
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

        GLchar* log = new GLchar[errorLength];
        glGetShaderInfoLog(vs, errorLength, 0, log);

        std::cerr << "Failed to compile the shader:\n" << log << std::endl;

        delete[] log;
        exit(1);
    }

    //=========================================================

    //����������� ������
    const char* fragmentShaderText =
        "#version 330\n"

        "out vec4 fragColor;\n"

        "void main()\n"
        "{\n"
        "    fragColor = vec4(0.5, 0.0, 1.0, 1.0);\n"
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

        GLchar* log = new GLchar[errorLength];
        glGetShaderInfoLog(fs, errorLength, 0, log);

        std::cerr << "Failed to compile the shader:\n" << log << std::endl;

        delete[] log;
        exit(1);
    }

    //=========================================================

    //������� ��������� ���������
    GLuint program = glCreateProgram();

    //����������� ��������� �������
    glAttachShader(program, vs);
    glAttachShader(program, fs);

    //������� ���������
    glLinkProgram(program);

    //��������� ������ ��������
    status = -1;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        GLint errorLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &errorLength);

        GLchar* log = new GLchar[errorLength];
        glGetProgramInfoLog(program, errorLength, 0, log);

        std::cerr << "Failed to link the program:\n" << log << std::endl;

        delete[] log;
        exit(1);
    }

    //=========================================================

    //���� ���������� (���� ���� �� �������)
    while (!glfwWindowShouldClose(window))
    {
        //��������� ������� ����� (����� ���������� ������� ��������� ������ ��� ��������� ������� ����������)
        glfwPollEvents();

        //�������� ������� ������ (����)
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        //������������� ���� ������ �� ���� ����� (����)
        glViewport(0, 0, width, height);

        //������� ���� ������ (����� ����� � ����� �������)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //���������� ��������� ���������
        glUseProgram(program);

        //���������� VertexArrayObject � ���������� ������������� ������
        glBindVertexArray(vao);

        //������ ������������� ������ (������� �� �������������, ����� 0, ���������� ������ 3)
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window); //����������� �������� � ������ ������
    }

    return 0;
}