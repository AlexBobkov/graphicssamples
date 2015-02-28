#include "Application.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#include <iostream>
#include <vector>

/**
��������� ����� (�������� GL_POINTS)
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

		//�������� ����������� �����
		glEnable(GL_POINT_SMOOTH);
		//�������� ��������� ������� ����� ����� ������
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
				
		cube = std::make_shared<Mesh>();
		cube->makeCube(0.5);
		cube->modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f));
		cube->primitiveType() = GL_POINTS;

		bunny = std::make_shared<Mesh>();
		bunny->loadFromFile("models/bunny.obj");
		bunny->modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		bunny->primitiveType() = GL_POINTS;

		//=========================================================

		_shaderProgram = ShaderProgram::createProgram("shaders3/shaderPoint.vert", "shaders3/shader.frag");

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