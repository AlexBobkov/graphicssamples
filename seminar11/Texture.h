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
Содержит cтатические методы для создания текстур
*/
class Texture
{
public:
	static GLuint loadTexture(std::string filename, bool withAlpha = false);
	static GLuint loadTextureWithMipmaps(std::string filename);
	static GLuint loadCubeTexture(std::string basefilename);
	static GLuint makeCustomTexture();
	static GLuint makeTextureBuffer(std::vector<glm::vec3>& positions);

protected:
	Texture() {}
};