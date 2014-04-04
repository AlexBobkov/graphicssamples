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
	_blurTexUniform = glGetUniformLocation(_programId, "blurTex");
	_exposureUniform = glGetUniformLocation(_programId, "exposure");
	_focalDistanceUniform = glGetUniformLocation(_programId, "focalDistance");
	_focalRangeUniform = glGetUniformLocation(_programId, "focalRange");

	_projMatrixInverseUniform = glGetUniformLocation(_programId, "projMatrixInverse");	
	_depthTexUniform = glGetUniformLocation(_programId, "depthTex");
}

void ToneMappingEffect::applyCommonUniforms() 
{
}

void ToneMappingEffect::applyModelSpecificUniforms()
{
	glUniform1i(_texUniform, _texUnit);
	glUniform1i(_bloomTexUniform, _bloomTexUnit);
	glUniform1i(_blurTexUniform, _blurTexUnit);
	glUniform1f(_exposureUniform, _exposure);
	glUniform1f(_focalDistanceUniform, _focalDistance);
	glUniform1f(_focalRangeUniform, _focalRange);

	glUniformMatrix4fv(_projMatrixInverseUniform, 1, GL_FALSE, glm::value_ptr(_projMatrixInverse));
	glUniform1i(_depthTexUniform, _depthTexUnit);
}
