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
Шейдер для отложенного рендеринга. Получает на вход 3 текстуры: нормаль, диффузный цвет и глубину.
*/
class DeferredRenderingMaterial: public Material
{
public:
	DeferredRenderingMaterial();

	void initialize() override;
	void applyCommonUniforms() override;
	void applyModelSpecificUniforms() override;

	void setViewMatrix(glm::mat4 mat) { _viewMatrix = mat; }

	void setViewMatrixInverse(glm::mat4 mat) { _viewMatrixInverse = mat; }
	void setProjMatrixInverse(glm::mat4 mat) { _projMatrixInverse = mat; }

	void setLightViewMatrix(glm::mat4 mat) { _lightViewMatrix = mat; }
	void setLightProjectionMatrix(glm::mat4 mat) { _lightProjMatrix = mat; }

	void setLightPos(glm::vec4 vec) { _lightPos = vec; }	
	void setAmbientColor(glm::vec3 vec) { _ambientColor = vec; }	
	void setDiffuseColor(glm::vec3 vec) { _diffuseColor = vec; }	
	void setSpecularColor(glm::vec3 vec) { _specularColor = vec; }	

	void setNormalsTexUnit(int unit) { _normalsTexUnit = unit; }
	void setDiffuseTexUnit(int unit) { _diffuseTexUnit = unit; }
	void setDepthTexUnit(int unit) { _depthTexUnit = unit; }
	void setShadowTexUnit(int unit) { _shadowTexUnit = unit; }

	void setAddShadow(bool addShadow) { _addShadow = addShadow; }

protected:	
	//====== идентификаторы uniform-переменных ======
	GLuint _viewMatrixUniform;

	GLuint _viewMatrixInverseUniform;
	GLuint _projMatrixInverseUniform;

	GLuint _lightViewMatrixUniform;
	GLuint _lightProjMatrixUniform;
	GLuint _lightScaleBiasMatrixUniform;

	GLuint _normalsTexUniform;
	GLuint _diffuseTexUniform;
	GLuint _depthTexUniform;
	GLuint _shadowTexUniform;

	GLuint _lightPosUniform;
	GLuint _ambientColorUniform;
	GLuint _diffuseColorUniform;
	GLuint _specularColorUniform;

	GLuint _addShadowUniform;

	glm::mat4 _viewMatrix;

	glm::mat4 _viewMatrixInverse;
	glm::mat4 _projMatrixInverse;

	glm::mat4 _lightViewMatrix;
	glm::mat4 _lightProjMatrix;
	glm::mat4 _lightScaleBiasMatrix;

	//текстурные юниты
	int _normalsTexUnit;
	int _diffuseTexUnit;
	int _depthTexUnit;
	int _shadowTexUnit;

	//параметры освещения
	glm::vec4 _lightPos; //in world space
	glm::vec3 _ambientColor;
	glm::vec3 _diffuseColor;
	glm::vec3 _specularColor;

	bool _addShadow;
};
