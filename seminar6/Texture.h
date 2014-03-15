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

/**
	Содержит cтатические методы для создания текстур
*/
class Texture
{
public:
	static GLuint loadTexture(std::string filename);
	static GLuint loadTextureWithMipmaps(std::string filename);
	static GLuint loadCubeTexture(std::string basefilename);
	static GLuint makeCustomTexture();

protected:
	Texture() {}
};