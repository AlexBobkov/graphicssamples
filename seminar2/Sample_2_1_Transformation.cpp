#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
Куб и кролик. Управление виртуальной камерой. Вращение кролика.
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
				
		//Создаем меш с помощью нового вспомогательного класса
		cube.makeCube(0.5);
		cube.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f));

		//Создаем ещё один меш с помощью нового вспомогательного класса
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

		glUseProgram(_shaderProgram); //Устанавливаем шейдер

		glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_camera.projMatrix)); //Загружаем на видеокарту матрицу проекции
		glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_camera.viewMatrix)); //Загружаем на видеокарту матрицу вида

		glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(cube.modelMatrix())); //Загружаем на видеокарту матрицу модели первого меша
		cube.draw(); //Рисуем первый меш

		float angle = (float)glfwGetTime();
		bunny.modelMatrix() = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0)), angle, glm::vec3(0.0f, 0.0f, 1.0f)); //Изменяем матрицу модели второго меша

		glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(bunny.modelMatrix())); //Загружаем на видеокарту матрицу модели второго меша
		bunny.draw(); //Рисуем второй меш
	}

    //В этом примере нет ГУИ
    void initGUI() override { }
    void updateGUI() override { }
    void drawGUI() override { }
};

int main()
{
	SampleApplication app;
	app.start();

	return 0;
}