#include "Application.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#include <iostream>
#include <vector>

/**
Точечный источник света
*/
class SampleApplication : public Application
{
public:
	MeshPtr cube;
	MeshPtr sphere;
	MeshPtr bunny;

	MeshPtr marker; //Меш - маркер для источника света

	//Идентификатор шейдерной программы
	GLuint _shaderProgram;
	GLuint _markerShaderProgram;
	
	//Идентификаторы uniform-переменных
	GLuint _modelMatrixUniform;
	GLuint _viewMatrixUniform;
	GLuint _projMatrixUniform;
	GLuint _normalToCameraMatrixUniform;

	GLuint _mvpMatrixUniform; //для шейдера для маркера

	GLuint _lightPosUniform;	
	GLuint _lightAmbientColorUniform;
	GLuint _lightDiffuseColorUniform;
	GLuint _lightSpecularColorUniform;

	GLuint _materialAmbientUniform;	
	GLuint _materialDiffuseUniform;
	GLuint _materialSpecularUniform;

	//переменные, которые содержат значения, которые будут записаны в uniform-переменные шейдеров
	glm::vec3 _lightPos; //in world space
	glm::vec3 _lightAmbientColor;
	glm::vec3 _lightDiffuseColor;
	glm::vec3 _lightSpecularColor;
		
	glm::vec3 _rabbitAmbientColor;
	glm::vec3 _rabbitDiffuseColor;
	glm::vec3 _rabbitSpecularColor;

	virtual void makeScene()
	{
		Application::makeScene();

		//=========================================================
		//Создание и загрузка мешей		
				
		cube = std::make_shared<Mesh>();
		cube->makeCube(0.5);
		cube->modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f));

		sphere = std::make_shared<Mesh>();
		sphere->makeSphere(0.5);
		sphere->modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f));

		bunny = std::make_shared<Mesh>();
		bunny->loadFromFile("models/bunny.obj");
		bunny->modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		marker = std::make_shared<Mesh>();
		marker->makeSphere(0.1);

		//=========================================================
		//Инициализация шейдеров
		
		_shaderProgram = ShaderProgram::createProgram("shaders4/specularPointLight.vert", "shaders4/specularPointLight.frag");
		_markerShaderProgram = ShaderProgram::createProgram("shaders4/marker.vert", "shaders4/marker.frag");

		//=========================================================
		//Инициализация uniform-переменных для преобразования координат

		_modelMatrixUniform = glGetUniformLocation(_shaderProgram, "modelMatrix");
		_viewMatrixUniform = glGetUniformLocation(_shaderProgram, "viewMatrix");
		_projMatrixUniform = glGetUniformLocation(_shaderProgram, "projectionMatrix");
		_normalToCameraMatrixUniform = glGetUniformLocation(_shaderProgram, "normalToCameraMatrix");

		_mvpMatrixUniform = glGetUniformLocation(_markerShaderProgram, "mvpMatrix");

		_viewMatrix = glm::lookAt(glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		_projMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.f);

		//=========================================================
		//Инициализация uniform-переменных для освещения

		_lightPosUniform = glGetUniformLocation(_shaderProgram, "light.pos");
		_lightAmbientColorUniform = glGetUniformLocation(_shaderProgram, "light.La");
		_lightDiffuseColorUniform = glGetUniformLocation(_shaderProgram, "light.Ld");
		_lightSpecularColorUniform = glGetUniformLocation(_shaderProgram, "light.Ls");
		_materialAmbientUniform = glGetUniformLocation(_shaderProgram, "material.Ka");
		_materialDiffuseUniform = glGetUniformLocation(_shaderProgram, "material.Kd");
		_materialSpecularUniform = glGetUniformLocation(_shaderProgram, "material.Ks");

		//Инициализация значений переменных освщения
		_lightPos = glm::vec3(0.0f, 1.0f, 0.8f) * 3.0f;
		_lightAmbientColor = glm::vec3(0.2, 0.2, 0.2);
		_lightDiffuseColor = glm::vec3(0.8, 0.8, 0.8);
		_lightSpecularColor = glm::vec3(1.0, 1.0, 1.0);

		//Инициализация материала кролика
		_rabbitAmbientColor = glm::vec3(1.0, 1.0, 0.0);
		_rabbitDiffuseColor = glm::vec3(1.0, 1.0, 0.0);
		_rabbitSpecularColor = glm::vec3(1.0, 1.0, 1.0);
	}

	virtual void initGUI()
	{
		Application::initGUI();

		TwAddVarRW(_bar, "LightPos", TW_TYPE_DIR3F, &_lightPos, " group=Light ");
		TwAddVarRW(_bar, "La", TW_TYPE_COLOR3F, &_lightAmbientColor, " group=Light label='ambient' ");
		TwAddVarRW(_bar, "Ld", TW_TYPE_COLOR3F, &_lightDiffuseColor, " group=Light label='diffuse' ");
		TwAddVarRW(_bar, "Ls", TW_TYPE_COLOR3F, &_lightSpecularColor, " group=Light label='specular' ");
		TwAddVarRW(_bar, "Ka", TW_TYPE_COLOR3F, &_rabbitAmbientColor, " group='Rabbit material' label='ambient' ");
		TwAddVarRW(_bar, "Kd", TW_TYPE_COLOR3F, &_rabbitDiffuseColor, " group='Rabbit material' label='diffuse' ");
		TwAddVarRW(_bar, "Ks", TW_TYPE_COLOR3F, &_rabbitSpecularColor, " group='Rabbit material' label='specular' ");
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
		glUniformMatrix4fv(_viewMatrixUniform, 1, GL_FALSE, glm::value_ptr(_viewMatrix));
		glUniformMatrix4fv(_projMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix));

		glUniform3fv(_lightPosUniform, 1, glm::value_ptr(_lightPos));
		glUniform3fv(_lightAmbientColorUniform, 1, glm::value_ptr(_lightAmbientColor));
		glUniform3fv(_lightDiffuseColorUniform, 1, glm::value_ptr(_lightDiffuseColor));
		glUniform3fv(_lightSpecularColorUniform, 1, glm::value_ptr(_lightSpecularColor));

		//Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
		{
			glm::mat3 normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * cube->modelMatrix())));
			glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(normalToCameraMatrix));

			glUniform3fv(_materialAmbientUniform, 1, glm::value_ptr(glm::vec3(0.0, 1.0, 0.0)));
			glUniform3fv(_materialDiffuseUniform, 1, glm::value_ptr(glm::vec3(0.0, 1.0, 0.0)));
			glUniform3fv(_materialSpecularUniform, 1, glm::value_ptr(glm::vec3(0.0, 1.0, 0.0)));

			glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(cube->modelMatrix()));
			cube->draw();
		}	

		{
			glm::mat3 normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * sphere->modelMatrix())));
			glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(normalToCameraMatrix));

			glUniform3fv(_materialAmbientUniform, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
			glUniform3fv(_materialDiffuseUniform, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
			glUniform3fv(_materialSpecularUniform, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));

			glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(sphere->modelMatrix()));
			sphere->draw();
		}

		{
			glm::mat3 normalToCameraMatrix = glm::transpose(glm::inverse(glm::mat3(_viewMatrix * bunny->modelMatrix())));
			glUniformMatrix3fv(_normalToCameraMatrixUniform, 1, GL_FALSE, glm::value_ptr(normalToCameraMatrix));

			glUniform3fv(_materialAmbientUniform, 1, glm::value_ptr(_rabbitAmbientColor));
			glUniform3fv(_materialDiffuseUniform, 1, glm::value_ptr(_rabbitDiffuseColor));
			glUniform3fv(_materialSpecularUniform, 1, glm::value_ptr(_rabbitSpecularColor));

			glUniformMatrix4fv(_modelMatrixUniform, 1, GL_FALSE, glm::value_ptr(bunny->modelMatrix()));
			bunny->draw();
		}

		//Рисуем маркер для источника света		
		{
			glUseProgram(_markerShaderProgram);
			glUniformMatrix4fv(_mvpMatrixUniform, 1, GL_FALSE, glm::value_ptr(_projMatrix * _viewMatrix * glm::translate(glm::mat4(1.0f), _lightPos)));
			marker->draw();
		}
	}
};

int main()
{
	SampleApplication app;
	app.start();

	return 0;
}