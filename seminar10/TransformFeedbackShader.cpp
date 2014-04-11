#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "TransformFeedbackShader.h"

TransformFeedbackShader::TransformFeedbackShader():
Material()
{
}

void TransformFeedbackShader::initialize()
{
	std::string vertFilename = "shaders10/tf.vert";	
	std::string fragFilename = "shaders10/particle.frag";	

	_programId = makeShaderProgram(vertFilename, fragFilename);
	
	const GLchar* Varyings[3];
	Varyings[0] = "position";
	Varyings[1] = "velocity";
	Varyings[2] = "particleTime";

	glTransformFeedbackVaryings(_programId, 3, Varyings, GL_SEPARATE_ATTRIBS);

	glLinkProgram(_programId);

	//Проверяем ошибки линковки
	int status = -1;
	glGetProgramiv(_programId, GL_LINK_STATUS, &status);
	if (status != GL_TRUE)
	{
		std::cerr << "Failed to link the program:\n";		

		GLint errorLength;
		glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &errorLength);

		GLchar* log = new GLchar[errorLength];
		glGetProgramInfoLog(_programId, errorLength, 0, log);

		std::cerr << log << std::endl;

		delete[] log;
		exit(1);
	}
	
	//=========================================================
	//Инициализация uniform-переменных для преобразования координат	
	_modelMatrixUniform = glGetUniformLocation(_programId, "modelMatrix");
	_viewMatrixUniform = glGetUniformLocation(_programId, "viewMatrix");
	_projMatrixUniform = glGetUniformLocation(_programId, "projectionMatrix");

	//=========================================================
	//Инициализация uniform-переменных для текстурирования
	_diffuseTexUniform = glGetUniformLocation(_programId, "tex");	

	//=========================================================
	//Инициализация прочих uniform-переменных
	_deltaTimeUniform = glGetUniformLocation(_programId, "deltaTime");
}

void TransformFeedbackShader::applyCommonUniforms() 
{
	glUniform1f(_deltaTimeUniform, _deltaTime);
	//glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
	//glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));
}

void TransformFeedbackShader::applyModelSpecificUniforms()
{
	//glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_modelMatrix));	

	//glUniform1i(_diffuseTexUniform, _diffuseTexUnit);	
}
