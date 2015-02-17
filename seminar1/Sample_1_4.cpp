#include "Application.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include <iostream>
#include <vector>

class SampleApplication : public Application
{
public:
	GLuint _vao;

	GLuint _shaderProgram;
	GLuint _projMatrixUniform;
	glm::mat4 _projMatrix;

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
		std::vector<float> vertices;
		addPoint(vertices, -0.3f, 0.3f, 0.0f);
		addPoint(vertices, 0.3f, 0.3f, 0.0f);
		addPoint(vertices, 0.3f, -0.3f, 0.0f);
		addPoint(vertices, -0.3f, -0.3f, 0.0f);
		addPoint(vertices, -0.3f, 0.3f, -1.0f);
		addPoint(vertices, -0.3f, -0.3f, -1.0f);
		addPoint(vertices, 0.3f, -0.3f, -1.0f);

		addColor(vertices, 1.0f, 0.0f, 0.0f, 1.0f);
		addColor(vertices, 1.0f, 0.0f, 0.0f, 1.0f);
		addColor(vertices, 1.0f, 0.0f, 0.0f, 1.0f);
		addColor(vertices, 0.0f, 0.0f, 1.0f, 1.0f);
		addColor(vertices, 1.0f, 1.0f, 0.0f, 1.0f);
		addColor(vertices, 1.0f, 1.0f, 0.0f, 1.0f);
		addColor(vertices, 0.0f, 1.0f, 0.0f, 1.0f);

		int Npoints = vertices.size() / 7;

		std::vector<unsigned short> indices = {
			0, 1, 2,
			0, 2, 3,
			4, 0, 3,
			4, 3, 5,
			5, 6, 2,
			5, 2, 3 };

		unsigned int vbo = 0;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		unsigned int ibo = 0;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), indices.data(), GL_STATIC_DRAW);

		_vao = 0;
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)(Npoints * 4 * 3));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		//=========================================================

		_shaderProgram = createProgram("shaders1/cube.vert", "shaders1/color.frag");

		//=========================================================

		_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");
		_projMatrix = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.f);

		glUseProgram(_shaderProgram);
		glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));
	}

	void draw() override
	{
		int width, height;
		glfwGetFramebufferSize(_window, &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(_shaderProgram); //Подключаем шейдер

		glBindVertexArray(_vao);
		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0); //Рисуем с помощью индексов
	}
};

int main()
{
	SampleApplication app;
	app.start();

	return 0;
}