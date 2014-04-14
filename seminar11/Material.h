#pragma once

#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
Базовый класс для шейдеров. Содержит код загрузки шейдеров.
*/
class Material
{
public:
	Material();	

	//Инициализация шейдера
	virtual void initialize() = 0;

	//Загрузка на видеокарту значений uniform-переменных, которые являются общими для всех 3д-моделей
	virtual void applyCommonUniforms() = 0;

	//Загрузка на видеокарту значений uniform-переменных, которые различаются между 3д-моделями
	virtual void applyModelSpecificUniforms() = 0;

	GLuint getProgramId() { return _programId; }

protected:
	GLuint makeShader(GLenum shaderType, std::string filename); //Читает текст шейдера из файла и создает объект
	GLuint makeShaderProgram(std::string vertFilename, std::string fragFilename, std::string geomFilename = "");

	GLuint _programId;
};