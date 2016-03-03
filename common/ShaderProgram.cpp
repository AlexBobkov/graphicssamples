#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>
#include <fstream>

void Shader::createFromFile(const std::string& filepath)
{
    //Читаем текст шейдера из файла
    std::ifstream shaderFile(filepath.c_str());
    if (shaderFile.fail())
    {
        std::cerr << "Failed to load shader file " << filepath << std::endl;
        exit(1);
    }
    std::string shaderFileContent((std::istreambuf_iterator<char>(shaderFile)), (std::istreambuf_iterator<char>()));
    shaderFile.close();

    createFromString(shaderFileContent);
}

void Shader::createFromString(const std::string& text)
{
    const char* shaderText = text.c_str();

    glShaderSource(_id, 1, &shaderText, NULL);

    glCompileShader(_id);

    //Проверяем ошибки компиляции
    int status = -1;
    glGetShaderiv(_id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        GLint errorLength;
        glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &errorLength);

        std::vector<char> errorMessage;
        errorMessage.resize(errorLength);

        glGetShaderInfoLog(_id, errorLength, 0, errorMessage.data());

        std::cerr << "Failed to compile the shader:\n" << errorMessage.data() << std::endl;

        exit(1);
    }
}

//===================================================================

void ShaderProgram::createProgram(const std::string& vertFilepath, const std::string& fragFilepath)
{
    ShaderPtr vs = std::make_shared<Shader>(GL_VERTEX_SHADER);
    vs->createFromFile(vertFilepath);
    attachShader(vs);

    ShaderPtr fs = std::make_shared<Shader>(GL_FRAGMENT_SHADER);
    fs->createFromFile(fragFilepath);
    attachShader(fs);

    linkProgram();
}

void ShaderProgram::attachShader(const ShaderPtr& shader)
{
    glAttachShader(_programId, shader->id());

    _shaders.push_back(shader);
}

void ShaderProgram::linkProgram()
{
    glLinkProgram(_programId);

    //Проверяем ошибки линковки
    int status = -1;
    glGetProgramiv(_programId, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
        GLint errorLength;
        glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &errorLength);

        std::vector<char> errorMessage;
        errorMessage.resize(errorLength);

        glGetProgramInfoLog(_programId, errorLength, 0, errorMessage.data());

        std::cerr << "Failed to link the program:\n" << errorMessage.data() << std::endl;

        exit(1);
    }
}
