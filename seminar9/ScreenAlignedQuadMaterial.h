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
Шейдер для прямоугольника с текстурой, который всегда смотрит в камеру
*/
class ScreenAlignedQuadMaterial: public Material
{
public:
	ScreenAlignedQuadMaterial();

	void setGrayscale(bool b) { _grayscale = b; _gamma = !b; }
	void setGamma(bool b) { _gamma = b; _grayscale = !b; }

	void initialize() override;
	void applyCommonUniforms() override;
	void applyModelSpecificUniforms() override;

	void setTexUnit(int unit) { _texUnit = unit; }

protected:	
	//====== идентификаторы uniform-переменных ======
	GLuint _texUniform;

	//текстурные юниты
	int _texUnit;

	bool _grayscale;
	bool _gamma;
};
