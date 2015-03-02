#include "Application.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#include <iostream>
#include <vector>

/**
TODO: ОПИСАНИЕ
*/
class SampleApplication : public Application
{
public:
	MeshPtr cube;
	MeshPtr bunny;

	GLuint _shaderProgram;
	//GLuint _modelMatrixUniform;	
	//GLuint _viewMatrixUniform;
	//GLuint _projMatrixUniform;
	//GLuint _timeUniform;

	//идентификаторы uniform-переменных
	GLuint _timeUniform;

	GLuint _modelMatrixUniform;
	GLuint _viewMatrixUniform;
	GLuint _projMatrixUniform;
	GLuint _normalToCameraMatrixUniform;

	GLuint _lightDirUniform;
	GLuint _lightPosUniform;

	GLuint _ambientColorUniform;
	GLuint _diffuseColorUniform;
	GLuint _specularColorUniform;
	GLuint _shininessUniform;
	GLuint _attenuationUniform;

	GLuint _materialUniform;	

	//переменные, которые содержат значения, которые будут записаны в uniform-переменные шейдеров
	glm::mat3 _normalToCameraMatrix;
	glm::vec4 _lightDir; //in world space
	glm::vec4 _lightPos; //in world space
	glm::vec3 _ambientColor;
	glm::vec3 _diffuseColor;
	glm::vec3 _specularColor;
	float _attenuation;

	//sphere
	//GLuint _sphereVao;
	//glm::mat4 _sphereModelMatrix;
	//int _sphereNumTris;
	float _sphereShininess;
	glm::vec3 _sphereMaterial;

	//cube
	//GLuint _cubeVao;
	//glm::mat4 _cubeModelMatrix;
	//int _cubeNumTris;
	float _cubeShininess;
	glm::vec3 _cubeMaterial;

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

		int demoNum = 7; //only with addNormalAttribute == true
		//1 - diffuse per vertex directinal light
		//2 - diffuse per vertex point light
		//3 - diffuse per fragment point light
		//4 - specular phong
		//5 - specular blinn
		//6 - materials
		//7 - attenuation

		std::string vertFilename = "shaders4/colored.vert";
		std::string fragFilename = "shaders4/colored.frag";

		if (demoNum == 1)
		{
			vertFilename = "shaders4/diffusePerVertex.vert";
			fragFilename = "shaders4/diffusePerVertex.frag";
		}
		else if (demoNum == 2)
		{
			vertFilename = "shaders4/diffusePerVertexPoint.vert";
			fragFilename = "shaders4/diffusePerVertexPoint.frag";
		}
		else if (demoNum == 3)
		{
			vertFilename = "shaders4/diffusePerFramentPoint.vert";
			fragFilename = "shaders4/diffusePerFramentPoint.frag";
		}
		else if (demoNum == 4)
		{
			vertFilename = "shaders4/specular.vert";
			fragFilename = "shaders4/specular.frag";
		}
		else if (demoNum == 5)
		{
			vertFilename = "shaders4/specularBlinn.vert";
			fragFilename = "shaders4/specularBlinn.frag";
		}
		else if (demoNum == 6)
		{
			vertFilename = "shaders4/specularBlinnMaterial.vert";
			fragFilename = "shaders4/specularBlinnMaterial.frag";
		}
		else if (demoNum == 7)
		{
			vertFilename = "shaders4/specularBlinnAttenuation.vert";
			fragFilename = "shaders4/specularBlinnAttenuation.frag";
		}

		_shaderProgram = ShaderProgram::createProgram(vertFilename, fragFilename);

		//_modelMatrixUniform = glGetUniformLocation(_shaderProgram, "modelMatrix");
		//_viewMatrixUniform = glGetUniformLocation(_shaderProgram, "viewMatrix");
		//_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");
		//_timeUniform = glGetUniformLocation(_shaderProgram, "time");

		//=========================================================
		//Инициализация uniform-переменных для преобразования координат

		_timeUniform = glGetUniformLocation(_shaderProgram, "time");
		_modelMatrixUniform = glGetUniformLocation(_shaderProgram, "modelMatrix");
		_viewMatrixUniform = glGetUniformLocation(_shaderProgram, "viewMatrix");
		_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");
		_normalToCameraMatrixUniform = glGetUniformLocation(_shaderProgram, "normalToCameraMatrix");

		_viewMatrix = glm::lookAt(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		_projMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);

		//=========================================================
		//Инициализация uniform-переменных для освещения

		_lightDirUniform = glGetUniformLocation(_shaderProgram, "lightDir");
		_lightPosUniform = glGetUniformLocation(_shaderProgram, "lightPos");
		_ambientColorUniform = glGetUniformLocation(_shaderProgram, "ambientColor");
		_diffuseColorUniform = glGetUniformLocation(_shaderProgram, "diffuseColor");
		_specularColorUniform = glGetUniformLocation(_shaderProgram, "specularColor");
		_shininessUniform = glGetUniformLocation(_shaderProgram, "shininessFactor");
		_materialUniform = glGetUniformLocation(_shaderProgram, "material");
		_attenuationUniform = glGetUniformLocation(_shaderProgram, "attenuation");

		//Инициализация значений переменных освщения
		_lightDir = glm::vec4(0.0f, 1.0f, 0.8f, 0.0f);
		_lightPos = glm::vec4(0.0f, 1.0f, 0.8f, 1.0f);
		_ambientColor = glm::vec3(0.2, 0.2, 0.2);
		_diffuseColor = glm::vec3(0.8, 0.8, 0.8);
		_specularColor = glm::vec3(0.25, 0.25, 0.25);

		_sphereShininess = 100.0f;
		_cubeShininess = 100.0f;

		_sphereMaterial = glm::vec3(1.0, 0.0, 0.0);
		_cubeMaterial = glm::vec3(0.0, 1.0, 0.0);

		_attenuation = 1.0f;
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
		glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
		glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));

		glUniform4fv(_lightDirUniform, 1, glm::value_ptr(_lightDir));
		glUniform4fv(_lightPosUniform, 1, glm::value_ptr(_lightPos));
		glUniform3fv(_ambientColorUniform, 1, glm::value_ptr(_ambientColor));
		glUniform3fv(_diffuseColorUniform, 1, glm::value_ptr(_diffuseColor));
		glUniform3fv(_specularColorUniform, 1, glm::value_ptr(_specularColor));
		glUniform1f(_attenuationUniform, _attenuation);


		//Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку

		_normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * cube->modelMatrix())));
		glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(_normalToCameraMatrix));

		glUniform3fv(_materialUniform, 1, glm::value_ptr(_sphereMaterial));
		glUniform1f(_shininessUniform, _sphereShininess);

		glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(cube->modelMatrix()));
		cube->draw();

		//===================================

		_normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * bunny->modelMatrix())));
		glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(_normalToCameraMatrix));

		glUniform3fv(_materialUniform, 1, glm::value_ptr(_cubeMaterial));
		glUniform1f(_shininessUniform, _cubeShininess);

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