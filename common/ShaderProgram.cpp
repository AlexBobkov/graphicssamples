#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>
#include <fstream>

GLuint ShaderProgram::createShader(GLenum shaderType, const std::string& filename)
{
    //Читаем текст шейдера из файла
    std::ifstream vertFile(filename.c_str());
    if (vertFile.fail())
    {
        std::cerr << "Failed to load shader file " << filename << std::endl;
        exit(1);
    }
    std::string vertFileContent((std::istreambuf_iterator<char>(vertFile)), (std::istreambuf_iterator<char>()));
    vertFile.close();

    const char* vertexShaderText = vertFileContent.c_str();

    //Создаем шейдерный объект в OpenGL
    GLuint vs = glCreateShader(shaderType);
    glShaderSource(vs, 1, &vertexShaderText, NULL);
    glCompileShader(vs);

    //Проверяем ошибки компиляции
    int status = -1;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        std::cerr << "Failed to compile the shader:\n";

        GLint errorLength;
        glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &errorLength);

        GLchar* log = new GLchar[errorLength];
        glGetShaderInfoLog(vs, errorLength, 0, log);

        std::cerr << log << std::endl;

        delete[] log;
        exit(1);
    }

    return vs;
}

void ShaderProgram::createProgram(const std::string& vertFilename, const std::string& fragFilename)
{
    GLuint vs = createShader(GL_VERTEX_SHADER, vertFilename);
    GLuint fs = createShader(GL_FRAGMENT_SHADER, fragFilename);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, fs);
    glAttachShader(shaderProgram, vs);
    glLinkProgram(shaderProgram);

    //Проверяем ошибки линковки
    int status = -1;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        std::cerr << "Failed to link the program:\n";

        GLint errorLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &errorLength);

        GLchar* log = new GLchar[errorLength];
        glGetProgramInfoLog(shaderProgram, errorLength, 0, log);

        std::cerr << log << std::endl;

        delete[] log;
        exit(1);
    }

    _shaderId = shaderProgram;
}

void ShaderProgram::createProgram(const std::string& vertFilename, const std::string& geomFilename, const std::string& fragFilename)
{
    GLuint vs = createShader(GL_VERTEX_SHADER, vertFilename);
    GLuint gs = createShader(GL_GEOMETRY_SHADER, geomFilename);
    GLuint fs = createShader(GL_FRAGMENT_SHADER, fragFilename);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, fs);
    glAttachShader(shaderProgram, gs);
    glAttachShader(shaderProgram, vs);
    glLinkProgram(shaderProgram);

    //Проверяем ошибки линковки
    int status = -1;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        std::cerr << "Failed to link the program:\n";

        GLint errorLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &errorLength);

        GLchar* log = new GLchar[errorLength];
        glGetProgramInfoLog(shaderProgram, errorLength, 0, log);

        std::cerr << log << std::endl;

        delete[] log;
        exit(1);
    }

    _shaderId = shaderProgram;
}

void ShaderProgram::createProgramForTransformFeedback(const std::string& vertFilename, const std::string& fragFilename, const std::vector<std::string>& attribs)
{
    GLuint vs = createShader(GL_VERTEX_SHADER, vertFilename);
    GLuint fs = createShader(GL_FRAGMENT_SHADER, fragFilename);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, fs);
    glAttachShader(shaderProgram, vs);

    //===============================================

    GLchar** attribStrings = new GLchar*[attribs.size()];
    for (unsigned int i = 0; i < attribs.size(); i++)
    {
        attribStrings[i] = const_cast<GLchar*>(attribs[i].c_str());
    }
    glTransformFeedbackVaryings(shaderProgram, attribs.size(), attribStrings, GL_SEPARATE_ATTRIBS);

    //===============================================

    glLinkProgram(shaderProgram);

    //Проверяем ошибки линковки
    int status = -1;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        std::cerr << "Failed to link the program:\n";

        GLint errorLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &errorLength);

        GLchar* log = new GLchar[errorLength];
        glGetProgramInfoLog(shaderProgram, errorLength, 0, log);

        std::cerr << log << std::endl;

        delete[] log;
        exit(1);
    }

    _shaderId = shaderProgram;
}

void ShaderProgram::setVec3UniformArray(const std::string& name, const std::vector<glm::vec3>& positions) const
{
    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); i++)
    {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
    }

    GLuint uniformLoc = glGetUniformLocation(_shaderId, name.c_str());
    glUniform3fv(uniformLoc, data.size() / 3, data.data());
}