#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "RenderToGBufferMaterial.h"

RenderToGBufferMaterial::RenderToGBufferMaterial()
{
}

void RenderToGBufferMaterial::initialize()
{
	std::string vertFilename = "shaders8/toGbuffer.vert";
	std::string fragFilename = "shaders8/toGbuffer.frag";

	_programId = makeShaderProgram(vertFilename, fragFilename);
		
	//=========================================================
	//Инициализация uniform-переменных для преобразования координат	
	_modelMatrixUniform = glGetUniformLocation(_programId, "modelMatrix");
	_viewMatrixUniform = glGetUniformLocation(_programId, "viewMatrix");
	_projMatrixUniform = glGetUniformLocation(_programId, "projectionMatrix");
	_normalToCameraMatrixUniform = glGetUniformLocation(_programId, "normalToCameraMatrix");	

	//=========================================================
	//Инициализация uniform-переменных для текстурирования
	_diffuseTexUniform = glGetUniformLocation(_programId, "diffuseTex");	
}

void RenderToGBufferMaterial::applyCommonUniforms() 
{
	glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
	glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));		
}

void RenderToGBufferMaterial::applyModelSpecificUniforms()
{
	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_modelMatrix));	

	glm::mat3 normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * _modelMatrix)));
	glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(normalToCameraMatrix));
		
	glUniform1i(_diffuseTexUniform, _diffuseTexUnit);
}
