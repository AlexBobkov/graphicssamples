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
	ShaderProgram _shader;
	ShaderProgram _markerShader;
	
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
		
		_shader.createProgram("shaders4/specularPointLight.vert", "shaders4/specularPointLight.frag");
		_markerShader.createProgram("shaders4/marker.vert", "shaders4/marker.frag");
		
		//=========================================================
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
		_shader.use();

		//Загружаем на видеокарту значения юниформ-переменных
		_shader.setMat4Uniform("viewMatrix", _viewMatrix);
		_shader.setMat4Uniform("projectionMatrix", _projMatrix);

		_shader.setVec3Uniform("light.pos", _lightPos);
		_shader.setVec3Uniform("light.La", _lightAmbientColor);
		_shader.setVec3Uniform("light.Ld", _lightDiffuseColor);
		_shader.setVec3Uniform("light.Ls", _lightSpecularColor);

		//Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
		{
			_shader.setMat4Uniform("modelMatrix", cube->modelMatrix());
			_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * cube->modelMatrix()))));

			_shader.setVec3Uniform("material.Ka", glm::vec3(0.0, 1.0, 0.0));
			_shader.setVec3Uniform("material.Kd", glm::vec3(0.0, 1.0, 0.0));
			_shader.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));

			cube->draw();
		}	

		{
			_shader.setMat4Uniform("modelMatrix", sphere->modelMatrix());
			_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * sphere->modelMatrix()))));

			_shader.setVec3Uniform("material.Ka", glm::vec3(1.0, 1.0, 1.0));
			_shader.setVec3Uniform("material.Kd", glm::vec3(1.0, 1.0, 1.0));
			_shader.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));

			sphere->draw();
		}

		{
			_shader.setMat4Uniform("modelMatrix", bunny->modelMatrix());
			_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * bunny->modelMatrix()))));

			_shader.setVec3Uniform("material.Ka", glm::vec3(_rabbitAmbientColor));
			_shader.setVec3Uniform("material.Kd", glm::vec3(_rabbitDiffuseColor));
			_shader.setVec3Uniform("material.Ks", glm::vec3(_rabbitSpecularColor));

			bunny->draw();
		}

		//Рисуем маркер для источника света		
		{
			_markerShader.use();
			_markerShader.setMat4Uniform("mvpMatrix", _projMatrix * _viewMatrix * glm::translate(glm::mat4(1.0f), _lightPos));
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