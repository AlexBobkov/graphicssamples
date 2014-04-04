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
Шейдер для Screen Space Ambient Occlusion
*/
class SSAOEffect: public Material
{
public:
	SSAOEffect();

	void initialize() override;
	void applyCommonUniforms() override;
	void applyModelSpecificUniforms() override;

	void setProjMatrix(glm::mat4 mat) { _projMatrix = mat; }
	void setProjMatrixInverse(glm::mat4 mat) { _projMatrixInverse = mat; }

	void setDepthTexUnit(int unit) { _depthTexUnit = unit; }
	void setRotateTexUnit(int unit) { _rotateTexUnit = unit; }

protected:	
	//====== идентификаторы uniform-переменных ======
	GLuint _projMatrixUniform;
	GLuint _projMatrixInverseUniform;

	GLuint _depthTexUniform;
	GLuint _rotateTexUniform;

	glm::mat4 _projMatrix;
	glm::mat4 _projMatrixInverse;

	int _depthTexUnit;
	int _rotateTexUnit;
};
