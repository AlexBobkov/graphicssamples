#include "Application.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#include <iostream>
#include <vector>

class SampleApplication : public Application
{
public:
	MeshPtr quad;

	GLuint _shaderProgram;
	GLuint _modelMatrixUniform;	
	GLuint _viewMatrixUniform;
	GLuint _projMatrixUniform;
	GLuint _timeUniform;

	void makeScene() override
	{
		Application::makeScene();

		quad = std::make_shared<Mesh>();
		quad->makeScreenAlignedQuad();

		//=========================================================

		std::string vertFilename = "shaders3/shaderQuad.vert";
		std::string fragFilename = "shaders3/shaderQuad.frag";

		bool mandelbrot = false;
		if (mandelbrot)
		{
			fragFilename = "shaders3/shaderQuadMandelbrot.frag";
		}

		_shaderProgram = ShaderProgram::createProgram(vertFilename, fragFilename);

		_modelMatrixUniform = glGetUniformLocation(_shaderProgram, "modelMatrix");
		_viewMatrixUniform = glGetUniformLocation(_shaderProgram, "viewMatrix");
		_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");
		_timeUniform = glGetUniformLocation(_shaderProgram, "time");
	}

	void draw() override
	{
		//Получаем текущие размеры экрана и выставлям вьюпорт
		int width, height;
		glfwGetFramebufferSize(_window, &width, &height);

		glViewport(0, 0, width, height);

		//Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Подключаем шейдер
		glUseProgram(_shaderProgram);

		//Рисуем квад
		quad->draw();
	}
};

int main()
{
	SampleApplication app;
	app.start();

	return 0;
}