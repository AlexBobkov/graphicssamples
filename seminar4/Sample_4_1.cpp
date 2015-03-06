#include "Application.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#include <iostream>
#include <vector>

/**
Направленный источник света
*/
class SampleApplication : public Application
{
public:
	MeshPtr cube;
	MeshPtr sphere;
	MeshPtr bunny;

	ShaderProgram _shader;

	//переменные, которые содержат значения, которые будут записаны в uniform-переменные шейдеров
	glm::vec3 _lightDir; //in world space
	glm::vec3 _lightAmbientColor;
	glm::vec3 _lightDiffuseColor;	
		
	glm::vec3 _rabbitAmbientColor;
	glm::vec3 _rabbitDiffuseColor;

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

		//=========================================================
		//Инициализация шейдеров
				
		_shader.createProgram("shaders4/diffuseDirectionalLight.vert", "shaders4/diffuseDirectionalLight.frag");

		//=========================================================
		//Инициализация значений переменных освщения
		_lightDir = glm::vec3(0.0f, 1.0f, 0.8f);
		_lightAmbientColor = glm::vec3(0.2, 0.2, 0.2);
		_lightDiffuseColor = glm::vec3(0.8, 0.8, 0.8);

		//Инициализация материала кролика
		_rabbitAmbientColor = glm::vec3(1.0, 1.0, 0.0);
		_rabbitDiffuseColor = glm::vec3(1.0, 1.0, 0.0);
	}

	virtual void initGUI()
	{
		Application::initGUI();

		TwAddVarRW(_bar, "LightDir", TW_TYPE_DIR3F, &_lightDir, " group=Light ");
		TwAddVarRW(_bar, "La", TW_TYPE_COLOR3F, &_lightAmbientColor, " group=Light label='ambient' ");
		TwAddVarRW(_bar, "Ld", TW_TYPE_COLOR3F, &_lightDiffuseColor, " group=Light label='diffuse' ");
		TwAddVarRW(_bar, "Ka", TW_TYPE_COLOR3F, &_rabbitAmbientColor, " group='Rabbit material' label='ambient' ");
		TwAddVarRW(_bar, "Kd", TW_TYPE_COLOR3F, &_rabbitDiffuseColor, " group='Rabbit material' label='diffuse' ");
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
		_shader.use();

		//Загружаем на видеокарту значения юниформ-переменных
		_shader.setMat4Uniform("viewMatrix", _viewMatrix);
		_shader.setMat4Uniform("projectionMatrix", _projMatrix);

		_shader.setVec3Uniform("light.dir", _lightDir);
		_shader.setVec3Uniform("light.La", _lightAmbientColor);
		_shader.setVec3Uniform("light.Ld", _lightDiffuseColor);
		
		//Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
		{
			_shader.setMat4Uniform("modelMatrix", cube->modelMatrix());
			_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * cube->modelMatrix()))));

			_shader.setVec3Uniform("material.Ka", glm::vec3(0.0, 1.0, 0.0));
			_shader.setVec3Uniform("material.Kd", glm::vec3(0.0, 1.0, 0.0));

			cube->draw();
		}	

		{
			_shader.setMat4Uniform("modelMatrix", sphere->modelMatrix());
			_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * sphere->modelMatrix()))));

			_shader.setVec3Uniform("material.Ka", glm::vec3(1.0, 1.0, 1.0));
			_shader.setVec3Uniform("material.Kd", glm::vec3(1.0, 1.0, 1.0));

			sphere->draw();
		}

		{
			_shader.setMat4Uniform("modelMatrix", bunny->modelMatrix());
			_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * bunny->modelMatrix()))));

			_shader.setVec3Uniform("material.Ka", glm::vec3(_rabbitAmbientColor));
			_shader.setVec3Uniform("material.Kd", glm::vec3(_rabbitDiffuseColor));

			bunny->draw();
		}
	}
};

int main()
{
	SampleApplication app;
	app.start();

	return 0;
}