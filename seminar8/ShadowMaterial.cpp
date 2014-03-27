#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "ShadowMaterial.h"

ShadowMaterial::ShadowMaterial()
{
}

void ShadowMaterial::initialize()
{
	std::string vertFilename = "shaders8/shadow.vert";
	std::string fragFilename = "shaders8/shadow.frag";

	_programId = makeShaderProgram(vertFilename, fragFilename);
		
	//=========================================================
	//Инициализация uniform-переменных для преобразования координат	
	_modelMatrixUniform = glGetUniformLocation(_programId, "modelMatrix");
	_viewMatrixUniform = glGetUniformLocation(_programId, "viewMatrix");
	_projMatrixUniform = glGetUniformLocation(_programId, "projectionMatrix");
	_normalToCameraMatrixUniform = glGetUniformLocation(_programId, "normalToCameraMatrix");	

	_lightViewMatrixUniform = glGetUniformLocation(_programId, "lightViewMatrix");
	_lightProjMatrixUniform = glGetUniformLocation(_programId, "lightProjectionMatrix");
	_lightScaleBiasMatrixUniform = glGetUniformLocation(_programId, "lightScaleBiasMatrix");

	//=========================================================
	//Инициализация uniform-переменных для освещения		
	_lightPosUniform = glGetUniformLocation(_programId, "lightPos");
	_ambientColorUniform = glGetUniformLocation(_programId, "ambientColor");
	_diffuseColorUniform = glGetUniformLocation(_programId, "diffuseColor");
	_specularColorUniform = glGetUniformLocation(_programId, "specularColor");	

	//=========================================================
	//Инициализация uniform-переменных свойств матириалов
	_shininessUniform = glGetUniformLocation(_programId, "shininessFactor");

	//=========================================================
	//Инициализация uniform-переменных для текстурирования
	_diffuseTexUniform = glGetUniformLocation(_programId, "diffuseTex");	
	_shadowTexUniform = glGetUniformLocation(_programId, "shadowTex");

	//=========================================================
	//Инициализация прочих uniform-переменных
	_timeUniform = glGetUniformLocation(_programId, "time");


	_lightScaleBiasMatrix = glm::scale(glm::translate(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5)), glm::vec3(0.5, 0.5, 0.5));
}

void ShadowMaterial::applyCommonUniforms() 
{
	glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
	glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));

	glUniformMatrix4fv(_lightViewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_lightViewMatrix));
	glUniformMatrix4fv(_lightProjMatrixUniform, 1, GL_FALSE, glm::value_ptr(_lightProjMatrix));
	glUniformMatrix4fv(_lightScaleBiasMatrixUniform, 1, GL_FALSE, glm::value_ptr(_lightScaleBiasMatrix));
		
	glUniform4fv(_lightPosUniform, 1, glm::value_ptr(_lightPos));
	glUniform3fv(_ambientColorUniform, 1, glm::value_ptr(_ambientColor));
	glUniform3fv(_diffuseColorUniform, 1, glm::value_ptr(_diffuseColor));
	glUniform3fv(_specularColorUniform, 1, glm::value_ptr(_specularColor));
}

void ShadowMaterial::applyModelSpecificUniforms()
{
	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_modelMatrix));	

	glm::mat3 normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * _modelMatrix)));
	glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(normalToCameraMatrix));
		
	glUniform1f(_shininessUniform, _shininess);	

	glUniform1i(_diffuseTexUniform, _diffuseTexUnit);
	glUniform1i(_shadowTexUniform, _shadowTexUnit);
}
