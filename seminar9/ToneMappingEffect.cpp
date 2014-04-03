#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "ToneMappingEffect.h"

ToneMappingEffect::ToneMappingEffect():
Material(),
	_texUnit(0),
	_exposure(1.0f)
{
}

void ToneMappingEffect::initialize()
{
	std::string vertFilename = "shaders8/quad.vert";
	std::string fragFilename = "shaders9/tonemapping.frag";

	_programId = makeShaderProgram(vertFilename, fragFilename);		

	//=========================================================
	//Инициализация uniform-переменных для текстурирования
	_texUniform = glGetUniformLocation(_programId, "tex");
	_bloomTexUniform = glGetUniformLocation(_programId, "bloomTex");
	_exposureUniform = glGetUniformLocation(_programId, "exposure");
}

void ToneMappingEffect::applyCommonUniforms() 
{
}

void ToneMappingEffect::applyModelSpecificUniforms()
{
	glUniform1i(_texUniform, _texUnit);
	glUniform1i(_bloomTexUniform, _bloomTexUnit);
	glUniform1f(_exposureUniform, _exposure);
}
