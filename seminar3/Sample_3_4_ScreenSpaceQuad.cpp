#include "Application.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#include <iostream>
#include <vector>

/**
Четырехугольник в Clip Space. Рисование во фрагментном шейдере. Фрактал Мандельброта.
*/
class SampleApplication : public Application
{
public:
	Mesh quad;

	GLuint _shaderProgram;
	GLuint _modelMatrixUniform;	
	GLuint _viewMatrixUniform;
	GLuint _projMatrixUniform;
	GLuint _timeUniform;

	virtual void makeScene()
	{
		Application::makeScene();

		quad.makeScreenAlignedQuad();

		//=========================================================

		std::string vertFilename = "shaders3/shaderQuad.vert";
		std::string fragFilename = "shaders3/shaderQuad.frag";

		bool mandelbrot = true;
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

	virtual void draw()
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
		quad.draw();
	}
};

int main()
{
	SampleApplication app;
	app.start();

	return 0;
}