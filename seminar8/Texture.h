#pragma once

#include <string>

#include <GL/glew.h>

/**
Загружает текстуры с диска в оперативную и затем в видеопамять.
Возвращает идентификатор текстурного объекта.
*/
class Texture
{
public:
	static GLuint loadTexture(const std::string& filename);

	static GLuint loadTextureWithMipmaps(const std::string& filename);

	static GLuint loadCubeTexture(const std::string& basefilename);

	static GLuint makeProceduralTexture();
};
