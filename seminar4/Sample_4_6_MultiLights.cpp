#include "Application.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#include <iostream>
#include <sstream>
#include <vector>

struct LightInfo
{
	glm::vec3 position; //Будем здесь хранить координаты в мировой системе координат, а при копировании в юниформ-переменную конвертировать в систему виртуальной камеры
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct MaterialInfo
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

const int LightNum = 3;

/**
Несколько источников света
*/
class SampleApplication : public Application
{
public:
	Mesh cube;
	Mesh sphere;
	Mesh bunny;

	Mesh marker; //Меш - маркер для источника света

	//Идентификатор шейдерной программы
	ShaderProgram _shader;
	ShaderProgram _markerShader;

	//Переменные для управления положением одного источника света
	float _lr;
	float _phi;
	float _theta;
		
	LightInfo _light[LightNum];
	MaterialInfo _material;

	virtual void makeScene()
	{
		Application::makeScene();

		//=========================================================
		//Создание и загрузка мешей		
				
		cube.makeCube(0.5);
		cube.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f));

		sphere.makeSphere(0.5, 100);
		sphere.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f));

		bunny.loadFromFile("models/bunny.obj");
		bunny.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		marker.makeSphere(0.1);

		//=========================================================
		//Инициализация шейдеров

		_shader.createProgram("shaders4/manyLights.vert", "shaders4/manyLights.frag");
		_markerShader.createProgram("shaders4/marker.vert", "shaders4/marker.frag");

		//=========================================================
		//Инициализация значений переменных освщения
		_lr = 3.0;
		_phi = 0.0;
		_theta = glm::pi<float>() * 0.25f;

		_light[0].position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
		_light[0].ambient = glm::vec3(0.2, 0.2, 0.2);
		_light[0].diffuse = glm::vec3(0.8, 0.8, 0.8);
		_light[0].specular = glm::vec3(1.0, 1.0, 1.0);

		_light[1].position = glm::vec3(0.0, 3.0, 3.0);
		_light[1].ambient = glm::vec3(0.2, 0.0, 0.0);
		_light[1].diffuse = glm::vec3(0.8, 0.0, 0.0);
		_light[1].specular = glm::vec3(1.0, 1.0, 1.0);

		_light[2].position = glm::vec3(0.0, -3.0, 3.0);
		_light[2].ambient = glm::vec3(0.0, 0.2, 0.0);
		_light[2].diffuse = glm::vec3(0.0, 0.8, 0.0);
		_light[2].specular = glm::vec3(1.0, 1.0, 1.0);

		//Инициализация материала кролика
		_material.ambient = glm::vec3(1.0, 1.0, 0.0);
		_material.diffuse = glm::vec3(1.0, 1.0, 0.0);
		_material.specular = glm::vec3(1.0, 1.0, 1.0);
		_material.shininess = 128.0f;
	}

	virtual void initGUI()
	{
		Application::initGUI();

		TwAddVarRW(_bar, "r", TW_TYPE_FLOAT, &_lr, "group=Light step=0.01 min=0.1 max=100.0");
		TwAddVarRW(_bar, "phi", TW_TYPE_FLOAT, &_phi, "group=Light step=0.01 min=0.0 max=6.28");
		TwAddVarRW(_bar, "theta", TW_TYPE_FLOAT, &_theta, "group=Light step=0.01 min=-1.57 max=1.57");
		TwAddVarRW(_bar, "La", TW_TYPE_COLOR3F, &_light[0].ambient, "group=Light label='ambient'");
		TwAddVarRW(_bar, "Ld", TW_TYPE_COLOR3F, &_light[0].diffuse, "group=Light label='diffuse'");
		TwAddVarRW(_bar, "Ls", TW_TYPE_COLOR3F, &_light[0].specular, "group=Light label='specular'");		
		TwAddVarRW(_bar, "Ka", TW_TYPE_COLOR3F, &_material.ambient, "group='Rabbit material' label='ambient'");
		TwAddVarRW(_bar, "Kd", TW_TYPE_COLOR3F, &_material.diffuse, "group='Rabbit material' label='diffuse'");
		TwAddVarRW(_bar, "Ks", TW_TYPE_COLOR3F, &_material.specular, "group='Rabbit material' label='specular'");
		TwAddVarRW(_bar, "shininess", TW_TYPE_FLOAT, &_material.shininess, "group='Rabbit material' min=0.1 max=255.0");
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

		_light[0].position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
		
		for (unsigned int i = 0; i < LightNum; i++)
		{
			std::ostringstream str;
			str << "light[" << i << "]";

			glm::vec3 lightPosCamSpace = glm::vec3(_viewMatrix * glm::vec4(_light[i].position, 1.0));

			_shader.setVec3Uniform(str.str() + ".pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
			_shader.setVec3Uniform(str.str() + ".La", _light[i].ambient);
			_shader.setVec3Uniform(str.str() + ".Ld", _light[i].diffuse);
			_shader.setVec3Uniform(str.str() + ".Ls", _light[i].specular);
		}

		//Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
		{
			_shader.setMat4Uniform("modelMatrix", cube.modelMatrix());
			_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * cube.modelMatrix()))));

			_shader.setVec3Uniform("material.Ka", glm::vec3(0.0, 1.0, 0.0));
			_shader.setVec3Uniform("material.Kd", glm::vec3(0.0, 1.0, 0.0));
			_shader.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
			_shader.setFloatUniform("material.shininess", _material.shininess);

			cube.draw();
		}

		{
			_shader.setMat4Uniform("modelMatrix", sphere.modelMatrix());
			_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * sphere.modelMatrix()))));

			_shader.setVec3Uniform("material.Ka", glm::vec3(1.0, 1.0, 1.0));
			_shader.setVec3Uniform("material.Kd", glm::vec3(1.0, 1.0, 1.0));
			_shader.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
			_shader.setFloatUniform("material.shininess", _material.shininess);

			sphere.draw();
		}

		{
			_shader.setMat4Uniform("modelMatrix", bunny.modelMatrix());
			_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * bunny.modelMatrix()))));

			_shader.setVec3Uniform("material.Ka", _material.ambient);
			_shader.setVec3Uniform("material.Kd", _material.diffuse);
			_shader.setVec3Uniform("material.Ks", _material.specular);
			_shader.setFloatUniform("material.shininess", _material.shininess);

			bunny.draw();
		}

		//Рисуем маркеры для всех источников света		
		{
			_markerShader.use();

			for (unsigned int i = 0; i < LightNum; i++)
			{
				_markerShader.setMat4Uniform("mvpMatrix", _projMatrix * _viewMatrix * glm::translate(glm::mat4(1.0f), _light[i].position));
                _markerShader.setVec4Uniform("color", glm::vec4(_light[i].diffuse, 1.0f));
				marker.draw();
			}
		}
	}
};

int main()
{
	SampleApplication app;
	app.start();

	return 0;
}