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

/**
Источник света. Содержит положение и интенсивности разных компонент освещения.
*/
class Light
{
public:
	Light();	

	glm::vec3 getLightPos() { return _lightPos; }	
	void setLightPos(glm::vec3 pos) { _lightPos = pos; }

	glm::vec4 getLightPos4() { return glm::vec4(_lightPos, 1.0); }

	glm::vec3 getAmbientColor() { return _ambientColor; }  
	void setAmbientColor(glm::vec3 color) { _ambientColor = color; }

	glm::vec3 getDiffuseColor() { return _diffuseColor; }  
	void setDiffuseColor(glm::vec3 color) { _diffuseColor = color; }

	glm::vec3 getSpecularColor() { return _specularColor; }  
	void setSpecularColor(glm::vec3 color) { _specularColor = color; }

protected:
	glm::vec3 _lightPos; //in world space
	glm::vec3 _ambientColor;
	glm::vec3 _diffuseColor;
	glm::vec3 _specularColor;
};
