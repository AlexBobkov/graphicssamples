#include "Application.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#include <iostream>
#include <vector>

/**
Вариант с затуханием света с расстоянием
*/
class SampleApplication : public Application
{
public:
	MeshPtr cube;
	MeshPtr sphere;
	MeshPtr bunny;

	MeshPtr marker; //Меш - маркер для источника света

	//Идентификатор шейдерной программы
	ShaderProgram _shaderPerFragment;
	ShaderProgram _markerShader;

	float _lr;
	float _phi;
	float _theta;

	//переменные, которые содержат значения, которые будут записаны в uniform-переменные шейдеров
	glm::vec3 _lightAmbientColor;
	glm::vec3 _lightDiffuseColor;
	glm::vec3 _lightSpecularColor;
	float _attenuation;

	glm::vec3 _rabbitAmbientColor;
	glm::vec3 _rabbitDiffuseColor;
	glm::vec3 _rabbitSpecularColor;
	float _rabbitShininess;

	virtual void makeScene()
	{
		Application::makeScene();

		//=========================================================
		//Создание и загрузка мешей		

		cube = std::make_shared<Mesh>();
		cube->makeCube(0.5);
		cube->modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f));

		sphere = std::make_shared<Mesh>();
		sphere->makeSphere(0.5, 100);
		sphere->modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f));

		bunny = std::make_shared<Mesh>();
		bunny->loadFromFile("models/bunny.obj");
		bunny->modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		marker = std::make_shared<Mesh>();
		marker->makeSphere(0.1);

		//=========================================================
		//Инициализация шейдеров

		_shaderPerFragment.createProgram("shaders4/specularAttenuationPointLightPerFragment.vert", "shaders4/specularAttenuationPointLightPerFragment.frag");
		_markerShader.createProgram("shaders4/marker.vert", "shaders4/marker.frag");

		//=========================================================
		//Инициализация значений переменных освщения
		_lr = 5.0;
		_phi = 0.0;
		_theta = glm::pi<float>() * 0.25f;

		_lightAmbientColor = glm::vec3(0.2, 0.2, 0.2);
		_lightDiffuseColor = glm::vec3(0.8, 0.8, 0.8);
		_lightSpecularColor = glm::vec3(1.0, 1.0, 1.0);
		_attenuation = 1.0f;

		//Инициализация материала кролика
		_rabbitAmbientColor = glm::vec3(1.0, 1.0, 0.0);
		_rabbitDiffuseColor = glm::vec3(1.0, 1.0, 0.0);
		_rabbitSpecularColor = glm::vec3(1.0, 1.0, 1.0);
		_rabbitShininess = 128.0f;
	}

	virtual void initGUI()
	{
		Application::initGUI();

		TwAddVarRW(_bar, "r", TW_TYPE_FLOAT, &_lr, "group=Light step=0.01 min=0.1 max=100.0");
		TwAddVarRW(_bar, "phi", TW_TYPE_FLOAT, &_phi, "group=Light step=0.01 min=0.0 max=6.28");
		TwAddVarRW(_bar, "theta", TW_TYPE_FLOAT, &_theta, "group=Light step=0.01 min=-1.57 max=1.57");
		TwAddVarRW(_bar, "La", TW_TYPE_COLOR3F, &_lightAmbientColor, " group=Light label='ambient' ");
		TwAddVarRW(_bar, "Ld", TW_TYPE_COLOR3F, &_lightDiffuseColor, " group=Light label='diffuse' ");
		TwAddVarRW(_bar, "Ls", TW_TYPE_COLOR3F, &_lightSpecularColor, " group=Light label='specular' ");
		TwAddVarRW(_bar, "attenuation", TW_TYPE_FLOAT, &_attenuation, " group=Light step=0.01 min=0.01 max=255.0");
		TwAddVarRW(_bar, "Ka", TW_TYPE_COLOR3F, &_rabbitAmbientColor, " group='Rabbit material' label='ambient' ");
		TwAddVarRW(_bar, "Kd", TW_TYPE_COLOR3F, &_rabbitDiffuseColor, " group='Rabbit material' label='diffuse' ");
		TwAddVarRW(_bar, "Ks", TW_TYPE_COLOR3F, &_rabbitSpecularColor, " group='Rabbit material' label='specular' ");
		TwAddVarRW(_bar, "shininess", TW_TYPE_FLOAT, &_rabbitShininess, " group='Rabbit material' min=0.1 max=255.0");
	}

	virtual void draw()
	{
		//Получаем текущие размеры экрана и выставлям вьюпорт
		int width, height;
		glfwGetFramebufferSize(_window, &width, &height);

		glViewport(0, 0, width, height);

		//Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 lightPos = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * _lr;

		//Подключаем шейдер		
		_shaderPerFragment.use();

		//Загружаем на видеокарту значения юниформ-переменных
		_shaderPerFragment.setMat4Uniform("viewMatrix", _viewMatrix);
		_shaderPerFragment.setMat4Uniform("projectionMatrix", _projMatrix);


		_shaderPerFragment.setVec3Uniform("light.pos", lightPos);
		_shaderPerFragment.setVec3Uniform("light.La", _lightAmbientColor);
		_shaderPerFragment.setVec3Uniform("light.Ld", _lightDiffuseColor);
		_shaderPerFragment.setVec3Uniform("light.Ls", _lightSpecularColor);
		_shaderPerFragment.setFloatUniform("light.attenuation", _attenuation);

		//Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
		{
			_shaderPerFragment.setMat4Uniform("modelMatrix", cube->modelMatrix());
			_shaderPerFragment.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * cube->modelMatrix()))));

			_shaderPerFragment.setVec3Uniform("material.Ka", glm::vec3(0.0, 1.0, 0.0));
			_shaderPerFragment.setVec3Uniform("material.Kd", glm::vec3(0.0, 1.0, 0.0));
			_shaderPerFragment.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
			_shaderPerFragment.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
			_shaderPerFragment.setFloatUniform("material.shininess", _rabbitShininess);

			cube->draw();
		}

		{
			_shaderPerFragment.setMat4Uniform("modelMatrix", sphere->modelMatrix());
			_shaderPerFragment.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * sphere->modelMatrix()))));

			_shaderPerFragment.setVec3Uniform("material.Ka", glm::vec3(1.0, 1.0, 1.0));
			_shaderPerFragment.setVec3Uniform("material.Kd", glm::vec3(1.0, 1.0, 1.0));
			_shaderPerFragment.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
			_shaderPerFragment.setFloatUniform("material.shininess", _rabbitShininess);

			sphere->draw();
		}

		{
			_shaderPerFragment.setMat4Uniform("modelMatrix", bunny->modelMatrix());
			_shaderPerFragment.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * bunny->modelMatrix()))));

			_shaderPerFragment.setVec3Uniform("material.Ka", glm::vec3(_rabbitAmbientColor));
			_shaderPerFragment.setVec3Uniform("material.Kd", glm::vec3(_rabbitDiffuseColor));
			_shaderPerFragment.setVec3Uniform("material.Ks", glm::vec3(_rabbitSpecularColor));
			_shaderPerFragment.setFloatUniform("material.shininess", _rabbitShininess);

			bunny->draw();
		}

		//Рисуем маркер для источника света		
		{
			_markerShader.use();
			_markerShader.setMat4Uniform("mvpMatrix", _projMatrix * _viewMatrix * glm::translate(glm::mat4(1.0f), lightPos));
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