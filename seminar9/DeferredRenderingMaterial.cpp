#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "DeferredRenderingMaterial.h"

DeferredRenderingMaterial::DeferredRenderingMaterial()
{
}

void DeferredRenderingMaterial::initialize()
{
	std::string vertFilename = "shaders9/deferred.vert";
	std::string fragFilename = "shaders9/deferred.frag";

	_programId = makeShaderProgram(vertFilename, fragFilename);		

	//=========================================================
	_viewMatrixUniform = glGetUniformLocation(_programId, "viewMatrix");

	_viewMatrixInverseUniform = glGetUniformLocation(_programId, "viewMatrixInverse");
	_projMatrixInverseUniform = glGetUniformLocation(_programId, "projMatrixInverse");

	_lightViewMatrixUniform = glGetUniformLocation(_programId, "lightViewMatrix");
	_lightProjMatrixUniform = glGetUniformLocation(_programId, "lightProjectionMatrix");
	_lightScaleBiasMatrixUniform = glGetUniformLocation(_programId, "lightScaleBiasMatrix");

	//Инициализация uniform-переменных
	_normalsTexUniform = glGetUniformLocation(_programId, "normalsTex");
	_diffuseTexUniform = glGetUniformLocation(_programId, "diffuseTex");
	_depthTexUniform = glGetUniformLocation(_programId, "depthTex");
	_shadowTexUniform = glGetUniformLocation(_programId, "shadowTex");
	
	//=========================================================
	//Инициализация uniform-переменных для освещения		
	_lightPosUniform = glGetUniformLocation(_programId, "lightPos");
	_ambientColorUniform = glGetUniformLocation(_programId, "ambientColor");
	_diffuseColorUniform = glGetUniformLocation(_programId, "diffuseColor");
	_specularColorUniform = glGetUniformLocation(_programId, "specularColor");

	_lightScaleBiasMatrix = glm::scale(glm::translate(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5)), glm::vec3(0.5, 0.5, 0.5));
}

void DeferredRenderingMaterial::applyCommonUniforms() 
{
	glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));

	glUniformMatrix4fv(_viewMatrixInverseUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrixInverse));
	glUniformMatrix4fv(_projMatrixInverseUniform, 1, GL_FALSE, glm::value_ptr(_projMatrixInverse));

	glUniformMatrix4fv(_lightViewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_lightViewMatrix));
	glUniformMatrix4fv(_lightProjMatrixUniform, 1, GL_FALSE, glm::value_ptr(_lightProjMatrix));
	glUniformMatrix4fv(_lightScaleBiasMatrixUniform, 1, GL_FALSE, glm::value_ptr(_lightScaleBiasMatrix));

	glUniform1i(_normalsTexUniform, _normalsTexUnit);
	glUniform1i(_diffuseTexUniform, _diffuseTexUnit);
	glUniform1i(_depthTexUniform, _depthTexUnit);
	glUniform1i(_shadowTexUniform, _shadowTexUnit);
}

void DeferredRenderingMaterial::applyModelSpecificUniforms()
{
	glUniform4fv(_lightPosUniform, 1, glm::value_ptr(_lightPos));
	glUniform3fv(_ambientColorUniform, 1, glm::value_ptr(_ambientColor));
	glUniform3fv(_diffuseColorUniform, 1, glm::value_ptr(_diffuseColor));
	glUniform3fv(_specularColorUniform, 1, glm::value_ptr(_specularColor));
}