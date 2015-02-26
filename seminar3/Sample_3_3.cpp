#include "Application.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#include <iostream>
#include <vector>

class SampleApplication : public Application
{
public:
	GLuint _vao;
	unsigned int _numVertices;

	GLuint _shaderProgram;
	GLuint _modelMatrixUniform;	
	GLuint _viewMatrixUniform;
	GLuint _projMatrixUniform;
	GLuint _timeUniform;

	void makeScene() override
	{
		Application::makeScene();

		_numVertices = 36;

		std::vector<float> vertices;
		for (unsigned int i = 0; i < _numVertices; i++)
		{
			vertices.push_back((float)i);
		}

		unsigned int vbo = 0;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		_vao = 0;
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, NULL);

		//=========================================================

		_shaderProgram = ShaderProgram::createProgram("shaders3/shaderLine.vert", "shaders3/shader.frag");

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

		//Загружаем на видеокарту значения юниформ-переменные: время и матрицы
		glUniform1f(_timeUniform, (float)glfwGetTime()); //передаем время в шейдер	

		glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));
		glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));

		//Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
		glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

		//Рисуем линию
		glBindVertexArray(_vao);
		glDrawArrays(GL_LINE_STRIP, 0, _numVertices);
	}
};

int main()
{
	SampleApplication app;
	app.start();

	return 0;
}