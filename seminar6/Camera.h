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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

/**
	Виртуальная камера. Хранит матрицы вида и проекции. Позволяет вращать виртуальной камерой
*/
class Camera
{
public:
	Camera();	

	glm::mat4 getViewMatrix() { return _viewMatrix; }  
	void setViewMatrix(glm::mat4& mat) { _viewMatrix = mat; }

	glm::mat4 getProjMatrix() { return _projMatrix; }  
	void setProjMatrix(glm::mat4& mat) { _projMatrix = mat; }

	glm::vec3 getCameraPos() { return _cameraPos; }  
	void setCameraPos(glm::vec3& mat) { _cameraPos = mat; }

	void update();

	void setWindowSize(int width, int height);

	//Функции для управления положением камеры
	void rotateLeft(bool b) { _rotateLeft = b; }
	void rotateRight(bool b) { _rotateRight = b; }

	void rotateUp(bool b) { _rotateUp = b; }
	void rotateDown(bool b) { _rotateDown = b; }

	void zoomUp(bool b) { _zoomUp = b; }
	void zoomDown(bool b) { _zoomDown = b; }

	void homePos();

protected:
	glm::mat4 _viewMatrix;
	glm::mat4 _projMatrix;
	glm::vec3 _cameraPos;

	bool _rotateLeft;
	bool _rotateRight;
	float _phiAng;

	bool _rotateUp;
	bool _rotateDown;
	float _thetaAng;

	bool _zoomUp;
	bool _zoomDown;
	float _distance;

	float _oldTime;
};