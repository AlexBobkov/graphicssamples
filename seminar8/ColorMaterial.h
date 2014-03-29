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

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "Material.h"

/**
Простой шейдер для рендера одноцветных объектов (например, маркеров для источников света)
*/
class ColorMaterial: public Material
{
public:
	ColorMaterial();

	void initialize() override;
	void applyCommonUniforms() override;
	void applyModelSpecificUniforms() override;

	void setModelMatrix(glm::mat4 mat) { _modelMatrix = mat; }
	void setViewMatrix(glm::mat4 mat) { _viewMatrix = mat; }
	void setProjectionMatrix(glm::mat4 mat) { _projMatrix = mat; }

	void setColor(glm::vec3 color) { _color = color; }

protected:	
	//====== идентификаторы uniform-переменных ======
	GLuint _modelMatrixUniform;
	GLuint _viewMatrixUniform;
	GLuint _projMatrixUniform;
	GLuint _colorUniform;

	//====== переменные, которые содержат значения, которые будут записаны в uniform-переменные шейдеров ======
	//матрицы
	glm::mat4 _modelMatrix;
	glm::mat4 _viewMatrix;
	glm::mat4 _projMatrix;

	glm::vec3 _color;
};
