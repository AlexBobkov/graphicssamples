#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "ScreenAlignedQuadMaterial.h"

ScreenAlignedQuadMaterial::ScreenAlignedQuadMaterial()
{
}

void ScreenAlignedQuadMaterial::initialize()
{
	std::string vertFilename = "shaders7/quad.vert";
	std::string fragFilename = "shaders7/quad.frag";

	_programId = makeShaderProgram(vertFilename, fragFilename);		
		
	//=========================================================
	//Инициализация uniform-переменных для текстурирования
	_texUniform = glGetUniformLocation(_programId, "tex");
}

void ScreenAlignedQuadMaterial::applyCommonUniforms() 
{
}

void ScreenAlignedQuadMaterial::applyModelSpecificUniforms()
{
	glUniform1i(_texUniform, _texUnit);
}
