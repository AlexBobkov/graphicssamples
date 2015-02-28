#include "Application.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#include <iostream>
#include <vector>

/**
Несколько примеров шейдеров
*/
class SampleApplication : public Application
{
public:
	MeshPtr cube;
	MeshPtr bunny;

	GLuint _shaderProgram;
	GLuint _modelMatrixUniform;	
	GLuint _viewMatrixUniform;
	GLuint _projMatrixUniform;
	GLuint _timeUniform;

	virtual void makeScene()
	{
		Application::makeScene();
				
		cube = std::make_shared<Mesh>();
		cube->makeCube(0.5);
		cube->modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f));

		bunny = std::make_shared<Mesh>();
		bunny->loadFromFile("models/bunny.obj");
		bunny->modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		//=========================================================

		const int demoNumber = 8; //1 - simple, 2 - mat, 3 - color, 5 - time, 6 - color time, 7 - time coords, 8 - discard

		std::string vertFilename = "shaders3/shader.vert";
		std::string fragFilename = "shaders3/shader.frag";

		if (demoNumber == 1)
		{
			vertFilename = "shaders3/simple.vert";
			fragFilename = "shaders3/simple.frag";
		}
		else if (demoNumber == 2)
		{
			vertFilename = "shaders3/simpleMat.vert";
			fragFilename = "shaders3/simple.frag";
		}
		else if (demoNumber == 3)
		{
			vertFilename = "shaders3/shader.vert";
			fragFilename = "shaders3/shader.frag";
		}
		else if (demoNumber == 5)
		{
			vertFilename = "shaders3/shaderTime.vert";
			fragFilename = "shaders3/shader.frag";
		}
		else if (demoNumber == 6)
		{
			vertFilename = "shaders3/shader.vert";
			fragFilename = "shaders3/shaderTime.frag";
		}
		else if (demoNumber == 7)
		{
			vertFilename = "shaders3/shaderTimeCoord.vert";
			fragFilename = "shaders3/shaderTimeCoord.frag";
		}
		else if (demoNumber == 8)
		{
			vertFilename = "shaders3/shader.vert";
			fragFilename = "shaders3/shaderDiscard.frag";
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

		//Загружаем на видеокарту значения юниформ-переменные: время и матрицы
		glUniform1f(_timeUniform, (float)glfwGetTime()); //передаем время в шейдер	

		glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));
		glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));

		//Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
		glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(cube->modelMatrix()));
		cube->draw();

		glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(bunny->modelMatrix()));
		bunny->draw();
	}
};

int main()
{
	SampleApplication app;
	app.start();

	return 0;
}