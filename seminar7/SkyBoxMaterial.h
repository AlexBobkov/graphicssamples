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
	Шейдер для фонового куба, который рисуется с помощью кубической текстуры
*/
class SkyBoxMaterial: public Material
{
public:
	SkyBoxMaterial();

	void initialize() override;
	void applyCommonUniforms() override;
	void applyModelSpecificUniforms() override;

	void setCameraPos(glm::vec3 vec) { _cameraPos = vec; }
	void setViewMatrix(glm::mat4 mat) { _viewMatrix = mat; }
	void setProjectionMatrix(glm::mat4 mat) { _projMatrix = mat; }	

	void setTexUnit(int unit) { _texUnit = unit; }
	
protected:	
	//====== идентификаторы uniform-переменных ======
	GLuint _cameraPosUniform;
	GLuint _viewMatrixUniform;
	GLuint _projMatrixUniform;	
		
	GLuint _texUniform;

	//====== переменные, которые содержат значения, которые будут записаны в uniform-переменные шейдеров ======	
	glm::vec3 _cameraPos;
	glm::mat4 _viewMatrix;
	glm::mat4 _projMatrix;

	//текстурные юниты
	int _texUnit;
};
