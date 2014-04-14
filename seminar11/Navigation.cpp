#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library

#include <AntTweakBar.h>

#include "Application.h"

//Функция обратного вызова для обработки нажатий на клавиатуре
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Application* app = (Application*)glfwGetWindowUserPointer(window);
	Camera& camera = app->getMainCamera();

	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		else if (key == GLFW_KEY_A)
		{
			camera.rotateLeft(true);
		}
		else if (key == GLFW_KEY_D)
		{
			camera.rotateRight(true);
		}
		else if (key == GLFW_KEY_W)
		{
			camera.rotateUp(true);
		}
		else if (key == GLFW_KEY_S)
		{
			camera.rotateDown(true);
		}
		else if (key == GLFW_KEY_R)
		{
			camera.zoomUp(true);
		}
		else if (key == GLFW_KEY_F)
		{
			camera.zoomDown(true);
		}
		else if (key == GLFW_KEY_SPACE)
		{
			camera.homePos();
		}
	}
	else if (action == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_A)
		{
			camera.rotateLeft(false);
		}
		else if (key == GLFW_KEY_D)
		{
			camera.rotateRight(false);
		}
		else if (key == GLFW_KEY_W)
		{
			camera.rotateUp(false);
		}
		else if (key == GLFW_KEY_S)
		{
			camera.rotateDown(false);
		}
		else if (key == GLFW_KEY_R)
		{
			camera.zoomUp(false);
		}
		else if (key == GLFW_KEY_F)
		{
			camera.zoomDown(false);
		}
	}

	TwEventKeyGLFW(key, action);
}
