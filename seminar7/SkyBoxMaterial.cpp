#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "SkyBoxMaterial.h"

SkyBoxMaterial::SkyBoxMaterial()
{
}

void SkyBoxMaterial::initialize()
{
	std::string vertFilename = "shaders6/skybox.vert";
	std::string fragFilename = "shaders6/skybox.frag";

	_programId = makeShaderProgram(vertFilename, fragFilename);
		
	//=========================================================
	//Инициализация uniform-переменных для преобразования координат	
	_cameraPosUniform = glGetUniformLocation(_programId, "cameraPos");
	_viewMatrixUniform = glGetUniformLocation(_programId, "viewMatrix");
	_projMatrixUniform = glGetUniformLocation(_programId, "projectionMatrix");
		
	//=========================================================
	//Инициализация uniform-переменных для текстурирования
	_texUniform = glGetUniformLocation(_programId, "cubeTex");
}

void SkyBoxMaterial::applyCommonUniforms() 
{
	glUniform3fv(_cameraPosUniform, 1, glm::value_ptr(_cameraPos));	
	glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
	glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));		
}

void SkyBoxMaterial::applyModelSpecificUniforms()
{
	glUniform1i(_texUniform, _texUnit);
}
