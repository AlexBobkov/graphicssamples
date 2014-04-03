#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "BloomEffect.h"

BloomEffect::BloomEffect():
Material(),
	_texUnit(0),
	_pass(1)
{
}

void BloomEffect::initialize()
{
	std::string vertFilename = "shaders8/quad.vert";
	std::string fragFilename;

	if (_pass == 1)
	{
		fragFilename = "shaders9/bright.frag";
	}
	else if (_pass == 2)
	{
		fragFilename = "shaders9/horizBlur.frag";
	}
	else
	{
		fragFilename = "shaders9/vertBlur.frag";
	}

	_programId = makeShaderProgram(vertFilename, fragFilename);		

	//=========================================================
	//Инициализация uniform-переменных для текстурирования
	_texUniform = glGetUniformLocation(_programId, "tex");
}

void BloomEffect::applyCommonUniforms() 
{
}

void BloomEffect::applyModelSpecificUniforms()
{
	glUniform1i(_texUniform, _texUnit);
}
