#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "ScreenAlignedQuadMaterial.h"

ScreenAlignedQuadMaterial::ScreenAlignedQuadMaterial():
Material(),
	_grayscale(false),
	_gamma(false)
{
}

void ScreenAlignedQuadMaterial::initialize()
{
	std::string vertFilename = "shaders8/quad.vert";
	std::string fragFilename = "shaders8/quad.frag";

	if (_grayscale)
	{
		fragFilename = "shaders9/grayscale.frag";
	}
	else if (_gamma)
	{
	}

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
