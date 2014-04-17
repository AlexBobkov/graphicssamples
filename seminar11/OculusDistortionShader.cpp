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
}

void OculusDistortionShader::applyCommonUniforms() 
{
}

void OculusDistortionShader::applyModelSpecificUniforms()
{
	glUniform1i(_texUniform, _texUnit);
}
