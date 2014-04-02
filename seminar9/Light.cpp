#define _USE_MATH_DEFINES
#include <math.h>

#include "Light.h"

Light::Light():
_lightPos(glm::vec3(10.0f, 10.0f, 1.0f)),
	_ambientColor(glm::vec3(0.2, 0.2, 0.2)),
	_diffuseColor(glm::vec3(0.8, 0.8, 0.8)),
	_specularColor(glm::vec3(0.5, 0.5, 0.5))
{
}