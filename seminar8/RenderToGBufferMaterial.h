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
Шейдер для рендеринга в G-буфер: нормали и диффузный цвет
*/
class RenderToGBufferMaterial: public Material
{
public:
	RenderToGBufferMaterial();

	void initialize() override;
	void applyCommonUniforms() override;
	void applyModelSpecificUniforms() override;

	void setModelMatrix(glm::mat4 mat) { _modelMatrix = mat; }
	void setViewMatrix(glm::mat4 mat) { _viewMatrix = mat; }
	void setProjectionMatrix(glm::mat4 mat) { _projMatrix = mat; }	

	void setDiffuseTexUnit(int unit) { _diffuseTexUnit = unit; }

protected:	
	//====== идентификаторы uniform-переменных ======
	GLuint _timeUniform;
	GLuint _modelMatrixUniform;
	GLuint _viewMatrixUniform;
	GLuint _projMatrixUniform;	
	GLuint _normalToCameraMatrixUniform;

	GLuint _diffuseTexUniform;

	//====== переменные, которые содержат значения, которые будут записаны в uniform-переменные шейдеров ======
	//матрицы
	glm::mat4 _modelMatrix;
	glm::mat4 _viewMatrix;
	glm::mat4 _projMatrix;

	//текстурные юниты
	int _diffuseTexUnit;
};
