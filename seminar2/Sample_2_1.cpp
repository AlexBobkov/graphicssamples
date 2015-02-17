#include "Application.h"

#include <iostream>
#include <vector>

class SampleApplication : public Application
{
public:
	GLuint _vao;

	glm::mat4 _modelMatrix;

	GLuint _shaderProgram;
	GLuint _modelMatrixUniform;	
	GLuint _viewMatrixUniform;
	GLuint _projMatrixUniform;

	void addPoint(std::vector<float>& vec, float x, float y, float z)
	{
		vec.push_back(x);
		vec.push_back(y);
		vec.push_back(z);
	}

	void addColor(std::vector<float>& vec, float r, float g, float b, float a)
	{
		vec.push_back(r);
		vec.push_back(g);
		vec.push_back(b);
		vec.push_back(a);
	}

	void makeScene() override
	{
		Application::makeScene();

		float size = 0.5f;

		std::vector<float> vertices;
		//front
		addPoint(vertices, size, -size, size);
		addPoint(vertices, size, size, size);
		addPoint(vertices, size, size, -size);

		addPoint(vertices, size, -size, size);
		addPoint(vertices, size, size, -size);
		addPoint(vertices, size, -size, -size);

		//left
		addPoint(vertices, -size, -size, size);
		addPoint(vertices, size, -size, size);
		addPoint(vertices, size, -size, -size);

		addPoint(vertices, -size, -size, size);
		addPoint(vertices, size, -size, -size);
		addPoint(vertices, -size, -size, -size);

		//top
		addPoint(vertices, -size, size, size);
		addPoint(vertices, size, size, size);
		addPoint(vertices, size, -size, size);

		addPoint(vertices, -size, size, size);
		addPoint(vertices, -size, -size, size);
		addPoint(vertices, size, -size, size);

		//back
		addPoint(vertices, -size, -size, size);
		addPoint(vertices, -size, size, -size);
		addPoint(vertices, -size, size, size);

		addPoint(vertices, -size, -size, size);
		addPoint(vertices, -size, -size, -size);
		addPoint(vertices, -size, size, -size);

		//left
		addPoint(vertices, -size, size, size);
		addPoint(vertices, size, size, -size);
		addPoint(vertices, size, size, size);

		addPoint(vertices, -size, size, size);
		addPoint(vertices, -size, size, -size);
		addPoint(vertices, +size, size, -size);

		//bottom
		addPoint(vertices, -size, size, -size);
		addPoint(vertices, size, -size, -size);
		addPoint(vertices, size, size, -size);

		addPoint(vertices, -size, size, -size);
		addPoint(vertices, size, -size, -size);
		addPoint(vertices, -size, -size, -size);

		addColor(vertices, 1.0, 0.0, 0.0, 1.0);
		addColor(vertices, 1.0, 0.0, 0.0, 1.0);
		addColor(vertices, 1.0, 0.0, 0.0, 1.0);

		addColor(vertices, 1.0, 0.0, 0.0, 1.0);
		addColor(vertices, 1.0, 0.0, 0.0, 1.0);
		addColor(vertices, 1.0, 0.0, 0.0, 1.0);

		addColor(vertices, 1.0, 1.0, 0.0, 1.0);
		addColor(vertices, 1.0, 1.0, 0.0, 1.0);
		addColor(vertices, 1.0, 1.0, 0.0, 1.0);

		addColor(vertices, 1.0, 1.0, 0.0, 1.0);
		addColor(vertices, 1.0, 1.0, 0.0, 1.0);
		addColor(vertices, 1.0, 1.0, 0.0, 1.0);

		addColor(vertices, 0.0, 1.0, 0.0, 1.0);
		addColor(vertices, 0.0, 1.0, 0.0, 1.0);
		addColor(vertices, 0.0, 1.0, 0.0, 1.0);

		addColor(vertices, 0.0, 1.0, 0.0, 1.0);
		addColor(vertices, 0.0, 1.0, 0.0, 1.0);
		addColor(vertices, 0.0, 1.0, 0.0, 1.0);

		addColor(vertices, 0.0, 0.0, 1.0, 1.0);
		addColor(vertices, 0.0, 0.0, 1.0, 1.0);
		addColor(vertices, 0.0, 0.0, 1.0, 1.0);

		addColor(vertices, 0.0, 0.0, 1.0, 1.0);
		addColor(vertices, 0.0, 0.0, 1.0, 1.0);
		addColor(vertices, 0.0, 0.0, 1.0, 1.0);

		addColor(vertices, 0.0, 1.0, 1.0, 1.0);
		addColor(vertices, 0.0, 1.0, 1.0, 1.0);
		addColor(vertices, 0.0, 1.0, 1.0, 1.0);

		addColor(vertices, 0.0, 1.0, 1.0, 1.0);
		addColor(vertices, 0.0, 1.0, 1.0, 1.0);
		addColor(vertices, 0.0, 1.0, 1.0, 1.0);

		addColor(vertices, 1.0, 0.0, 1.0, 1.0);
		addColor(vertices, 1.0, 0.0, 1.0, 1.0);
		addColor(vertices, 1.0, 0.0, 1.0, 1.0);

		addColor(vertices, 1.0, 0.0, 1.0, 1.0);
		addColor(vertices, 1.0, 0.0, 1.0, 1.0);
		addColor(vertices, 1.0, 0.0, 1.0, 1.0);

		int Npoints = vertices.size() / 7;

		unsigned int vbo = 0;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		_vao = 0;
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(Npoints * 4 * 3));

		//=========================================================

		_modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

		//=========================================================

		_shaderProgram = createProgram("shaders2/shader.vert", "shaders2/shader.frag");		

		_modelMatrixUniform = glGetUniformLocation(_shaderProgram, "modelMatrix");
		_viewMatrixUniform = glGetUniformLocation(_shaderProgram, "viewMatrix");
		_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");
	}

	void draw() override
	{
		int width, height;
		glfwGetFramebufferSize(_window, &width, &height);

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(_shaderProgram);

		glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(_modelMatrix));
		glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
		glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));

		glBindVertexArray(_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36); //Рисуем 3 грани куба (6 треугольников)
	}
};

int main()
{
	SampleApplication app;
	app.start();

	return 0;
}