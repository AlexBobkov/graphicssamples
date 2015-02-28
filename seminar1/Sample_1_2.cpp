#include "Application.h"

#include <iostream>

class SampleApplication : public Application
{
public:
	GLuint _vao;

	GLuint _shaderProgram;
	GLuint _projMatrixUniform;	

	virtual void makeScene()
	{
		Application::makeScene();

		float points[] =
		{
			0.0f, 0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			-0.5f, -0.5f, 0.0f,
			1.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f, 1.0f,
		};

		//Копируем буфер с атрибутами на видеокарту
		//Сначала в буфере идут координаты, потом цвета вершин
		unsigned int vbo = 0;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, 21 * sizeof(float), points, GL_STATIC_DRAW);

		//Описываем какие буферы относятся к нашему мешу, сколько у него вершинных атрибутов и их настройки
		//Здесь 1 буфер содержит 2 атрибута
		_vao = 0;
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)36);

		//=========================================================

		_shaderProgram = createProgram("shaders1/triangle.vert", "shaders1/color.frag");

		_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");		
	}

	virtual void draw()
	{
		int width, height;
		glfwGetFramebufferSize(_window, &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(_shaderProgram); //Подключаем шейдер

		glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));

		glBindVertexArray(_vao); //Подключаем VertexArray с настойками буферов
		glDrawArrays(GL_TRIANGLES, 0, 3); //Рисуем треугольник
	}
};

int main()
{
	SampleApplication app;
	app.start();

	return 0;
}