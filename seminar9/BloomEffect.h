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
Шейдер для эффекта Bloom
*/
class BloomEffect: public Material
{
public:
	BloomEffect();

	void initialize() override;
	void applyCommonUniforms() override;
	void applyModelSpecificUniforms() override;

	void setPassNumber(int pass) { _pass = pass; }

	void setTexUnit(int unit) { _texUnit = unit; }
	void setTexSize(glm::vec2 size) { _texSize = size; }

protected:	
	//====== идентификаторы uniform-переменных ======
	GLuint _texUniform;
	GLuint _texSizeUniform;
	
	//текстурные юниты
	int _texUnit;
	glm::vec2 _texSize;
	
	int _pass;
};
