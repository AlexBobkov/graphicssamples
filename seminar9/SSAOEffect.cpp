#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "SSAOEffect.h"

SSAOEffect::SSAOEffect()
{
}

void SSAOEffect::initialize()
{
	std::string vertFilename = "shaders9/ssao.vert";
	std::string fragFilename = "shaders9/ssao.frag";

	_programId = makeShaderProgram(vertFilename, fragFilename);		

	//=========================================================
	_projMatrixUniform = glGetUniformLocation(_programId, "projMatrix");
	_projMatrixInverseUniform = glGetUniformLocation(_programId, "projMatrixInverse");

	//Инициализация uniform-переменных
	_depthTexUniform = glGetUniformLocation(_programId, "depthTex");
	_rotateTexUniform = glGetUniformLocation(_programId, "rotateTex");
}

void SSAOEffect::applyCommonUniforms() 
{
	glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));
	glUniformMatrix4fv(_projMatrixInverseUniform, 1, GL_FALSE, glm::value_ptr(_projMatrixInverse));

	glUniform1i(_depthTexUniform, _depthTexUnit);
	glUniform1i(_rotateTexUniform, _rotateTexUnit);
}

void SSAOEffect::applyModelSpecificUniforms()
{
}