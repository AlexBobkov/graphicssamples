#pragma once

#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "Material.h"

/**
Шейдер для корректировки искажений, вносимых линзой Oculus
*/
class OculusDistortionShader: public Material
{
public:
	OculusDistortionShader();

	void initialize() override;
	void applyCommonUniforms() override;
	void applyModelSpecificUniforms() override;

	void setTexUnit(int unit) { _texUnit = unit; }

	void setLensCenter(glm::vec2 v) { _lensCenter = v; }
	void setScreenCenter(glm::vec2 v) { _screenCenter = v; }
	void setScale(glm::vec2 v) { _scale = v; }
	void setScaleIn(glm::vec2 v) { _scaleIn = v; }
	void setWarpParams(glm::vec4 v) { _warpParams = v; }

protected:	
	//====== идентификаторы uniform-переменных ======
	GLuint _texUniform;
	GLuint _lensCenterUniform;
	GLuint _screenCenterUniform;
	GLuint _scaleUniform;
	GLuint _scaleInUniform;
	GLuint _warpParamsUniform;

	//текстурные юниты
	int _texUnit;

	glm::vec2 _lensCenter;
	glm::vec2 _screenCenter;
	glm::vec2 _scale;
	glm::vec2 _scaleIn;
	glm::vec4 _warpParams;
};
