#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "OculusDistortionShader.h"

OculusDistortionShader::OculusDistortionShader():
Material()
{
}

void OculusDistortionShader::initialize()
{
	std::string vertFilename = "shaders11/distortion.vert";
	std::string fragFilename = "shaders11/distortion.frag";

	_programId = makeShaderProgram(vertFilename, fragFilename);		

	//=========================================================
	//Инициализация uniform-переменных для текстурирования
	_texUniform = glGetUniformLocation(_programId, "tex");

	_lensCenterUniform = glGetUniformLocation(_programId, "LensCenter");
	_screenCenterUniform = glGetUniformLocation(_programId, "ScreenCenter");
	_scaleUniform = glGetUniformLocation(_programId, "Scale");
	_scaleInUniform = glGetUniformLocation(_programId, "ScaleIn");
	_warpParamsUniform = glGetUniformLocation(_programId, "HmdWarpParam");
}

void OculusDistortionShader::applyCommonUniforms() 
{
}

void OculusDistortionShader::applyModelSpecificUniforms()
{
	glUniform1i(_texUniform, _texUnit);

	glUniform2fv(_lensCenterUniform, 1, glm::value_ptr(_lensCenter));
	glUniform2fv(_screenCenterUniform, 1, glm::value_ptr(_screenCenter));
	glUniform2fv(_scaleUniform, 1, glm::value_ptr(_scale));
	glUniform2fv(_scaleInUniform, 1, glm::value_ptr(_scaleIn));
	glUniform4fv(_warpParamsUniform, 1, glm::value_ptr(_warpParams));
}
