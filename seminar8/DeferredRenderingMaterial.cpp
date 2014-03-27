#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "DeferredRenderingMaterial.h"

DeferredRenderingMaterial::DeferredRenderingMaterial()
{
}

void DeferredRenderingMaterial::initialize()
{
	std::string vertFilename = "shaders8/deferred.vert";
	std::string fragFilename = "shaders8/deferred.frag";

	_programId = makeShaderProgram(vertFilename, fragFilename);		

	//=========================================================
	_viewMatrixUniform = glGetUniformLocation(_programId, "viewMatrix");
	_projMatrixInverseUniform = glGetUniformLocation(_programId, "projMatrixInverse");

	//Инициализация uniform-переменных
	_normalsTexUniform = glGetUniformLocation(_programId, "normalsTex");
	_diffuseTexUniform = glGetUniformLocation(_programId, "diffuseTex");
	_depthTexUniform = glGetUniformLocation(_programId, "depthTex");

	//=========================================================
	//Инициализация uniform-переменных для освещения		
	_lightPosUniform = glGetUniformLocation(_programId, "lightPos");
	_ambientColorUniform = glGetUniformLocation(_programId, "ambientColor");
	_diffuseColorUniform = glGetUniformLocation(_programId, "diffuseColor");
	_specularColorUniform = glGetUniformLocation(_programId, "specularColor");
}

void DeferredRenderingMaterial::applyCommonUniforms() 
{
	glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
	glUniformMatrix4fv(_projMatrixInverseUniform, 1, GL_FALSE, glm::value_ptr(_projMatrixInverse));

	glUniform1i(_normalsTexUniform, _normalsTexUnit);
	glUniform1i(_diffuseTexUniform, _diffuseTexUnit);
	glUniform1i(_depthTexUniform, _depthTexUnit);

	glUniform4fv(_lightPosUniform, 1, glm::value_ptr(_lightPos));
	glUniform3fv(_ambientColorUniform, 1, glm::value_ptr(_ambientColor));
	glUniform3fv(_diffuseColorUniform, 1, glm::value_ptr(_diffuseColor));
	glUniform3fv(_specularColorUniform, 1, glm::value_ptr(_specularColor));
}

void DeferredRenderingMaterial::applyModelSpecificUniforms()
{
}