#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "CommonMaterial.h"

CommonMaterial::CommonMaterial():
Material(),
	_vertFilename("shaders8/common.vert")
{
}

void CommonMaterial::initialize()
{
	std::string fragFilename = "shaders8/common.frag";

	_programId = makeShaderProgram(_vertFilename, fragFilename);

	//=========================================================
	//Инициализация uniform-переменных для преобразования координат	
	_modelMatrixUniform = glGetUniformLocation(_programId, "modelMatrix");
	_viewMatrixUniform = glGetUniformLocation(_programId, "viewMatrix");
	_projMatrixUniform = glGetUniformLocation(_programId, "projectionMatrix");
	_normalToCameraMatrixUniform = glGetUniformLocation(_programId, "normalToCameraMatrix");	

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

	//=========================================================
	//Инициализация прочих uniform-переменных
	_timeUniform = glGetUniformLocation(_programId, "time");

	_positionsUniform = glGetUniformLocation(_programId, "positions");
}

void CommonMaterial::setPositions(std::vector<glm::vec3>& positions)
{
	for (unsigned int i = 0; i < positions.size(); i++)
	{
		_positions.push_back(positions[i].x);
		_positions.push_back(positions[i].y);
		_positions.push_back(positions[i].z);
	}

	glUseProgram(_programId);
	glUniform3fv(_positionsUniform, _positions.size() / 3, _positions.data());
	glUseProgram(0);
}

void CommonMaterial::applyCommonUniforms() 
{
	glUniform1f(_timeUniform, _time);
	glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
	glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));

	glUniform4fv(_lightPosUniform, 1, glm::value_ptr(_lightPos));
	glUniform3fv(_ambientColorUniform, 1, glm::value_ptr(_ambientColor));
	glUniform3fv(_diffuseColorUniform, 1, glm::value_ptr(_diffuseColor));
	glUniform3fv(_specularColorUniform, 1, glm::value_ptr(_specularColor));
}

void CommonMaterial::applyModelSpecificUniforms()
{
	glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_modelMatrix));	

	glm::mat3 normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * _modelMatrix)));
	glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(normalToCameraMatrix));

	glUniform1f(_shininessUniform, _shininess);	

	glUniform1i(_diffuseTexUniform, _diffuseTexUnit);	
}
