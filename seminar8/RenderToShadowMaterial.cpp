#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "RenderToShadowMaterial.h"

RenderToShadowMaterial::RenderToShadowMaterial()
{
}

void RenderToShadowMaterial::initialize()
{
	std::string vertFilename = "shaders8/toshadow.vert";
	std::string fragFilename = "shaders8/toshadow.frag";

	_programId = makeShaderProgram(vertFilename, fragFilename);
		
	//=========================================================
	//Инициализация uniform-переменных для преобразования координат	
	_modelMatrixUniform = glGetUniformLocation(_programId, "modelMatrix");
	_viewMatrixUniform = glGetUniformLocation(_programId, "viewMatrix");
	_projMatrixUniform = glGetUniformLocation(_programId, "projectionMatrix");
}

void RenderToShadowMaterial::applyCommonUniforms() 
{
	glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
	glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));		
}

void RenderToShadowMaterial::applyModelSpecificUniforms()
{
	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_modelMatrix));	
}
