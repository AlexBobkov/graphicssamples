#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "ColorMaterial.h"

ColorMaterial::ColorMaterial()
{
}

void ColorMaterial::initialize()
{
	std::string vertFilename = "shaders8/color.vert";
	std::string fragFilename = "shaders8/color.frag";

	_programId = makeShaderProgram(vertFilename, fragFilename);
		
	//=========================================================
	//Инициализация uniform-переменных для преобразования координат	
	_modelMatrixUniform = glGetUniformLocation(_programId, "modelMatrix");
	_viewMatrixUniform = glGetUniformLocation(_programId, "viewMatrix");
	_projMatrixUniform = glGetUniformLocation(_programId, "projectionMatrix");

	_colorUniform = glGetUniformLocation(_programId, "color");
}

void ColorMaterial::applyCommonUniforms() 
{
	glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
	glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));		
}

void ColorMaterial::applyModelSpecificUniforms()
{
	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_modelMatrix));		
	glUniform3fv(_colorUniform, 1, glm::value_ptr(_color));
}
