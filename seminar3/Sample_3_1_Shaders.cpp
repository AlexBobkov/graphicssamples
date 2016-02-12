#include "Application.h"

#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
��������� �������� ��������
*/
class SampleApplication : public Application
{
public:
	Mesh cube;
	Mesh bunny;

	GLuint _shaderProgram;
	GLuint _modelMatrixUniform;	
	GLuint _viewMatrixUniform;
	GLuint _projMatrixUniform;
	GLuint _timeUniform;

	virtual void makeScene()
	{
		Application::makeScene();
				
		cube.makeCube(0.5);
		cube.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f));

		bunny.loadFromFile("models/bunny.obj");
		bunny.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

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

        ShaderProgram sp;
        sp.createProgram(vertFilename, fragFilename);
        _shaderProgram = sp.id();

		_modelMatrixUniform = glGetUniformLocation(_shaderProgram, "modelMatrix");
		_viewMatrixUniform = glGetUniformLocation(_shaderProgram, "viewMatrix");
		_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");
		_timeUniform = glGetUniformLocation(_shaderProgram, "time");
	}

	virtual void draw()
	{
		//�������� ������� ������� ������ � ��������� �������
		int width, height;
		glfwGetFramebufferSize(_window, &width, &height);

		glViewport(0, 0, width, height);

		//������� ������ ����� � ������� �� ����������� ���������� ����������� �����
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//���������� ������
		glUseProgram(_shaderProgram);

		//��������� �� ���������� �������� �������-����������: ����� � �������
		glUniform1f(_timeUniform, (float)glfwGetTime()); //�������� ����� � ������	

		glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));
		glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));

		//��������� �� ���������� ������� ������ ����� � ��������� ���������
		glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(cube.modelMatrix()));
		cube.draw();

		glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(bunny.modelMatrix()));
		bunny.draw();
	}
};

int main()
{
	SampleApplication app;
	app.start();

	return 0;
}