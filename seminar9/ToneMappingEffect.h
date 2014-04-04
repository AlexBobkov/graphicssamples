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
Шейдер для эффекта Tone Mapping
*/
class ToneMappingEffect: public Material
{
public:
	ToneMappingEffect();

	void initialize() override;
	void applyCommonUniforms() override;
	void applyModelSpecificUniforms() override;

	void setExposure(float e) { _exposure = e; }
	void setTexUnit(int unit) { _texUnit = unit; }
	void setBloomTexUnit(int unit) { _bloomTexUnit = unit; }
	void setBlurTexUnit(int unit) { _blurTexUnit = unit; }

	void setFocalDistance(float f) { _focalDistance = f; }
	void setFocalRange(float f) { _focalRange = f; }

	void setDepthTexUnit(int unit) { _depthTexUnit = unit; }
	void setProjMatrixInverse(glm::mat4 mat) { _projMatrixInverse = mat; }

protected:	
	//====== идентификаторы uniform-переменных ======
	GLuint _texUniform;
	GLuint _bloomTexUniform;
	GLuint _blurTexUniform;
	GLuint _exposureUniform;
	GLuint _focalDistanceUniform;
	GLuint _focalRangeUniform;

	GLuint _projMatrixInverseUniform;
	GLuint _depthTexUniform;

	//текстурные юниты
	int _texUnit;
	int _bloomTexUnit;
	int _blurTexUnit;
	int _depthTexUnit;
	float _exposure;

	float _focalDistance;
	float _focalRange;

	glm::mat4 _projMatrixInverse;
};
