#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "ParticleMaterial.h"

ParticleMaterial::ParticleMaterial():
Material(),
	_vertFilename("shaders10/particle.vert")
{
}

void ParticleMaterial::initialize()
{
	std::string fragFilename = "shaders10/particle.frag";

	_programId = makeShaderProgram(_vertFilename, fragFilename);

	//=========================================================
	//Инициализация uniform-переменных для преобразования координат	
	_modelMatrixUniform = glGetUniformLocation(_programId, "modelMatrix");
	_viewMatrixUniform = glGetUniformLocation(_programId, "viewMatrix");
	_projMatrixUniform = glGetUniformLocation(_programId, "projectionMatrix");

	//=========================================================
	//Инициализация uniform-переменных для текстурирования
	_diffuseTexUniform = glGetUniformLocation(_programId, "tex");	

	//=========================================================
	//Инициализация прочих uniform-переменных
	_timeUniform = glGetUniformLocation(_programId, "time");
}

void ParticleMaterial::applyCommonUniforms() 
{
	glUniform1f(_timeUniform, _time);
	glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
	glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));
}

void ParticleMaterial::applyModelSpecificUniforms()
{
	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_modelMatrix));	

	glUniform1i(_diffuseTexUniform, _diffuseTexUnit);	
}
