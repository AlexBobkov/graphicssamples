#include "Application.h"

#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
��� � ������. ���������� ����������� �������. �������� �������.
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

	virtual void makeScene()
	{
		Application::makeScene();
				
		//������� ��� � ������� ������ ���������������� ������
		cube.makeCube(0.5);
		cube.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f));

		//������� ��� ���� ��� � ������� ������ ���������������� ������
		bunny.loadFromFile("models/bunny.obj");
		bunny.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		//=========================================================

        ShaderProgram sp;
        sp.createProgram("shaders2/shader.vert", "shaders2/shader.frag");
        _shaderProgram = sp.id();

		_modelMatrixUniform = glGetUniformLocation(_shaderProgram, "modelMatrix");
		_viewMatrixUniform = glGetUniformLocation(_shaderProgram, "viewMatrix");
		_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");
	}

	virtual void draw()
	{
		int width, height;
		glfwGetFramebufferSize(_window, &width, &height);

		glViewport(0, 0, width, height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(_shaderProgram); //������������� ������

		glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix)); //��������� �� ���������� ������� ��������
		glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix)); //��������� �� ���������� ������� ����

		glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(cube.modelMatrix())); //��������� �� ���������� ������� ������ ������� ����
		cube.draw(); //������ ������ ���

		float angle = (float)glfwGetTime();
		bunny.modelMatrix() = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0)), angle, glm::vec3(0.0f, 0.0f, 1.0f)); //�������� ������� ������ ������� ����

		glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(bunny.modelMatrix())); //��������� �� ���������� ������� ������ ������� ����
		bunny.draw(); //������ ������ ���
	}
};

int main()
{
	SampleApplication app;
	app.start();

	return 0;
}