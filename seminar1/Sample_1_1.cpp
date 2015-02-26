#include "Application.h"

#include <iostream>

class SampleApplication : public Application
{
public:
	GLuint _vao;

	GLuint _shaderProgram;
	GLuint _projMatrixUniform;

	void makeScene() override
	{
		Application::makeScene();

		float points[] =
		{
			0.0f, 0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			-0.5f, -0.5f, 0.0f
		};

		//�������� ����� � ������������ �� ����������
		GLuint vbo = 0;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

		//��������� ����� ������ ��������� � ������ ����, ������� � ���� ��������� ��������� � �� ���������
		_vao = 0;
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		//=========================================================

		_shaderProgram = createProgram("shaders1/simple.vert", "shaders1/simple.frag");

		_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");		
	}

	void draw() override
	{
		int width, height;
		glfwGetFramebufferSize(_window, &width, &height);
		
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(_shaderProgram); //���������� ������

		glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));

		glBindVertexArray(_vao); //���������� VertexArray � ���������� �������
		glDrawArrays(GL_TRIANGLES, 0, 3); //������ �����������
	}
};

int main()
{
	SampleApplication app;
	app.start();

	return 0;
}