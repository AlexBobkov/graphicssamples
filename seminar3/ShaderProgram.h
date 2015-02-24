#pragma once

#include <GL/glew.h>

#include <string>

/**
Создает шейдерную программу из 2х файлов: с вершинным и фрагментым шейдерами
*/
class ShaderProgram
{
public:
	/**
	Вспомогательная функция для создания шейдерной программы из 2х шейдеров: вершинного и фрагментного
	*/
	static GLuint createProgram(const std::string& vertFilename, const std::string& fragFilename);

protected:
	/**
	Вспомогательная функция для загрузки текста шейдера из файла и создания шейдерного объекта
	*/
	static GLuint createShader(GLenum shaderType, const std::string& filename);
};
