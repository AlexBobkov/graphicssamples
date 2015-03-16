#include "Application.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Texture.h"

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

/**
Пример с текстурированием разных 3д-моделей
*/
class SampleApplication : public Application
{
public:
	Mesh cube;
	Mesh sphere;
	Mesh bunny;
	Mesh ground;

	Mesh marker; //Меш - маркер для источника света

	//Идентификатор шейдерной программы
	ShaderProgram _shader;
	ShaderProgram _markerShader;

	//Переменные для управления положением одного источника света
	float _lr;
	float _phi;
	float _theta;

	LightInfo _light;

	GLuint _worldTexId;
	GLuint _brickTexId;
	GLuint _grassTexId;
	GLuint _chessTexId;
	GLuint _myTexId;

	GLuint _sampler;

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

		ground.makeGroundPlane(5.0f, 2.0f);

		marker.makeSphere(0.1);

		//=========================================================
		//Инициализация шейдеров

		_shader.createProgram("shaders5/texture.vert", "shaders5/texture.frag");
		_markerShader.createProgram("shaders5/marker.vert", "shaders5/marker.frag");

		//=========================================================
		//Инициализация значений переменных освщения
		_lr = 3.0;
		_phi = 0.0;
		_theta = glm::pi<float>() * 0.25f;

		_light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
		_light.ambient = glm::vec3(0.2, 0.2, 0.2);
		_light.diffuse = glm::vec3(0.8, 0.8, 0.8);
		_light.specular = glm::vec3(1.0, 1.0, 1.0);

		//=========================================================
		//Загрузка и создание текстур
		_worldTexId = Texture::loadTexture("images/earth_global.jpg");
		_brickTexId = Texture::loadTexture("images/brick.jpg");
		_grassTexId = Texture::loadTexture("images/grass.jpg");
		_chessTexId = Texture::loadTextureWithMipmaps("images/chess.dds");
		_myTexId = Texture::makeProceduralTexture();

		//=========================================================
		//Инициализация сэмплера, объекта, который хранит параметры чтения из текстуры
		glGenSamplers(1, &_sampler);
		glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	virtual void initGUI()
	{
		Application::initGUI();

		TwAddVarRW(_bar, "r", TW_TYPE_FLOAT, &_lr, "group=Light step=0.01 min=0.1 max=100.0");
		TwAddVarRW(_bar, "phi", TW_TYPE_FLOAT, &_phi, "group=Light step=0.01 min=0.0 max=6.28");
		TwAddVarRW(_bar, "theta", TW_TYPE_FLOAT, &_theta, "group=Light step=0.01 min=-1.57 max=1.57");
		TwAddVarRW(_bar, "La", TW_TYPE_COLOR3F, &_light.ambient, "group=Light label='ambient'");
		TwAddVarRW(_bar, "Ld", TW_TYPE_COLOR3F, &_light.diffuse, "group=Light label='diffuse'");
		TwAddVarRW(_bar, "Ls", TW_TYPE_COLOR3F, &_light.specular, "group=Light label='specular'");
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

		_light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
		glm::vec3 lightPosCamSpace = glm::vec3(_viewMatrix * glm::vec4(_light.position, 1.0));

		_shader.setVec3Uniform("light.pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
		_shader.setVec3Uniform("light.La", _light.ambient);
		_shader.setVec3Uniform("light.Ld", _light.diffuse);
		_shader.setVec3Uniform("light.Ls", _light.specular);

		glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0
		glBindTexture(GL_TEXTURE_2D, _worldTexId);
		glBindSampler(0, _sampler);
		_shader.setIntUniform("diffuseTex", 0);

		//Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
		{
			_shader.setMat4Uniform("modelMatrix", cube.modelMatrix());
			_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * cube.modelMatrix()))));

			cube.draw();
		}

		{
			_shader.setMat4Uniform("modelMatrix", sphere.modelMatrix());
			_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * sphere.modelMatrix()))));

			sphere.draw();
		}

		{
			_shader.setMat4Uniform("modelMatrix", bunny.modelMatrix());
			_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * bunny.modelMatrix()))));

			bunny.draw();
		}

		//{
		//	_shader.setMat4Uniform("modelMatrix", ground.modelMatrix());
		//	_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * ground.modelMatrix()))));

		//	ground.draw();
		//}

		//Рисуем маркеры для всех источников света		
		{
			_markerShader.use();

			_markerShader.setMat4Uniform("mvpMatrix", _projMatrix * _viewMatrix * glm::translate(glm::mat4(1.0f), _light.position));
			marker.draw();
		}

		//Отсоединяем сэмплер и шейдерную программу
		glBindSampler(0, 0);
		glUseProgram(0);
	}
};

int main()
{
	SampleApplication app;
	app.start();

	return 0;
}