#pragma once

#include <string>

#include <GL/glew.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
Загружает текстуры с диска в оперативную и затем в видеопамять.
Возвращает идентификатор текстурного объекта.
*/
class Texture
{
public:
    static GLuint loadTexture(const std::string& filename, bool gamma = false, bool withAlpha = false);

	static GLuint loadTextureWithMipmaps(const std::string& filename);

	static GLuint loadCubeTexture(const std::string& basefilename);

	static GLuint makeProceduralTexture();

    static GLuint makeTextureBuffer(std::vector<glm::vec3>& positions);
};
