#define _USE_MATH_DEFINES
#include <math.h>

#include "Camera.h"

Camera::Camera():
_rotateLeft(false),
	_rotateRight(false),
	_phiAng(0.0f),
	_rotateUp(false),
	_rotateDown(false),
	_thetaAng(0.0f),
	_zoomUp(false),
	_zoomDown(false),
	_distance(5.0f),
	_oldTime(0.0)
{
	_viewMatrix = glm::lookAt(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	_projMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);
}

void Camera::setWindowSize(int width, int height)
{
	_projMatrix = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.f);
}

void Camera::update()
{
	float dt = (float)glfwGetTime() - _oldTime;
	_oldTime = (float)glfwGetTime();

	float speed = 1.0f;

	if (_rotateLeft)
	{
		_phiAng -= speed * dt;
	}
	if (_rotateRight)
	{
		_phiAng += speed * dt;
	}
	if (_rotateUp)
	{
		_thetaAng += speed * dt;
	}
	if (_rotateDown)
	{
		_thetaAng -= speed * dt;
	}
	if (_zoomUp)
	{
		_distance += _distance * dt;
	}
	if (_zoomDown)
	{
		_distance -= _distance * dt;
	}

	_thetaAng = glm::clamp(_thetaAng, -(float)M_PI * 0.45f, (float)M_PI * 0.45f);
	_distance = glm::clamp(_distance, 0.5f, 500.0f);
	
	_cameraPos = glm::vec3(glm::cos(_phiAng) * glm::cos(_thetaAng), glm::sin(_phiAng) * glm::cos(_thetaAng), glm::sin(_thetaAng)) * _distance;
	_viewMatrix = glm::lookAt(_cameraPos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Camera::homePos()
{
	_phiAng = 0.0f;
	_thetaAng = (float)M_PI * 0.05f;
	_distance = 20.0f;

	_cameraPos = glm::vec3(glm::cos(_phiAng) * glm::cos(_thetaAng), glm::sin(_phiAng) * glm::cos(_thetaAng), glm::sin(_thetaAng)) * _distance;
	_viewMatrix = glm::lookAt(_cameraPos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}