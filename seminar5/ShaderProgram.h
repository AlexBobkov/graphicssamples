#pragma once

#include <string>

#include <GL/glew.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
Создает шейдерную программу из 2х файлов: с вершинным и фрагментым шейдерами
*/
class ShaderProgram
{
public:
	/**
	Создает шейдерную программу из 2х шейдеров: вершинного и фрагментного
	*/
	void createProgram(const std::string& vertFilename, const std::string& fragFilename);

	void use() const
	{
		glUseProgram(_shaderId);
	}

    void setIntUniform(const std::string& name, const int& value) const
	{
		GLuint uniformLoc = glGetUniformLocation(_shaderId, name.c_str());
		glUniform1i(uniformLoc, value);
	}

    void setFloatUniform(const std::string& name, const float& value) const
	{
		GLuint uniformLoc = glGetUniformLocation(_shaderId, name.c_str());
		glUniform1f(uniformLoc, value);
	}

    void setVec3Uniform(const std::string& name, const glm::vec3& vec) const
	{
		GLuint uniformLoc = glGetUniformLocation(_shaderId, name.c_str());
		glUniform3fv(uniformLoc, 1, glm::value_ptr(vec));
	}

    void setVec4Uniform(const std::string& name, const glm::vec4& vec) const
	{
		GLuint uniformLoc = glGetUniformLocation(_shaderId, name.c_str());
		glUniform4fv(uniformLoc, 1, glm::value_ptr(vec));
	}

    void setMat3Uniform(const std::string& name, const glm::mat3& mat) const
	{
		GLuint uniformLoc = glGetUniformLocation(_shaderId, name.c_str());
		glUniformMatrix3fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mat));
	}

    void setMat4Uniform(const std::string& name, const glm::mat4& mat) const
	{
		GLuint uniformLoc = glGetUniformLocation(_shaderId, name.c_str());
		glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(mat));
	}

protected:
	/**
	Вспомогательная функция для загрузки текста шейдера из файла и создания шейдерного объекта
	*/
	GLuint createShader(GLenum shaderType, const std::string& filename);

	GLuint _shaderId;
};
