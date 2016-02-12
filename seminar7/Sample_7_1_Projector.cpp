#include <Application.hpp>
#include <Mesh.hpp>
#include <ShaderProgram.hpp>
#include <Texture.hpp>

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
Пример с проективной текстурой
*/
class SampleApplication : public Application
{
public:
	Mesh cube;
	Mesh sphere;
	Mesh bunny;
	Mesh ground;
	Mesh backgroundCube;

	Mesh marker; //Меш - маркер для источника света

	//Идентификатор шейдерной программы
	ShaderProgram _commonShader;
	ShaderProgram _markerShader;
	ShaderProgram _skyboxShader;
	ShaderProgram _projectorShader;

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
	GLuint _cubeTexId;

	GLuint _sampler;
	GLuint _cubeTexSampler;
	GLuint _projSampler;

	float _projR;
	float _projPhi;
	float _projTheta;

	CameraInfo _projCamera; //Для управления проектором можно использовать те же настройки, что и для виртуальной камеры

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

		backgroundCube.makeCube(10.0f);

		//=========================================================
		//Инициализация шейдеров

		_commonShader.createProgram("shaders6/common.vert", "shaders6/common.frag");
		_markerShader.createProgram("shaders4/marker.vert", "shaders4/marker.frag");
		_skyboxShader.createProgram("shaders6/skybox.vert", "shaders6/skybox.frag");
		_projectorShader.createProgram("shaders7/projector.vert", "shaders7/projector.frag");

		//=========================================================
		//Инициализация значений переменных освщения
		_lr = 10.0;
		_phi = 2.65f;
		_theta = 0.48f;

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
		_cubeTexId = Texture::loadCubeTexture("images/cube");

		//=========================================================
		//Инициализация сэмплера, объекта, который хранит параметры чтения из текстуры
		glGenSamplers(1, &_sampler);
		glSamplerParameteri(_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glGenSamplers(1, &_cubeTexSampler);
		glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(_cubeTexSampler, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glGenSamplers(1, &_projSampler);
		glSamplerParameteri(_projSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_projSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(_projSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glSamplerParameteri(_projSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		//=========================================================
		//Инициализация проектора

		_projR = 4.0f;
		_projPhi = 0.0;
		_projTheta = 1.0;

		glm::vec3 projPos = glm::vec3(glm::cos(_projPhi) * glm::cos(_projTheta), glm::sin(_projPhi) * glm::cos(_projTheta), glm::sin(_projTheta)) * _projR;

		_projCamera.viewMatrix = glm::lookAt(projPos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		_projCamera.projMatrix = glm::perspective(glm::radians(25.0f), 1.0f, 0.1f, 100.f);
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

		TwAddVarRW(_bar, "Projector phi", TW_TYPE_FLOAT, &_projPhi, "group=Projector step=0.01");
		TwAddVarRW(_bar, "Projector theta", TW_TYPE_FLOAT, &_projTheta, "group=Projector step=0.01");
	}

	virtual void draw()
	{
		//Получаем текущие размеры экрана и выставлям вьюпорт
		int width, height;
		glfwGetFramebufferSize(_window, &width, &height);

		glViewport(0, 0, width, height);

		//Очищаем буферы цвета и глубины от результатов рендеринга предыдущего кадра
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//====== РИСУЕМ ОСНОВНЫЕ ОБЪЕКТЫ СЦЕНЫ ======
		_projectorShader.use();

		//Загружаем на видеокарту значения юниформ-переменных
		_projectorShader.setMat4Uniform("viewMatrix", _camera.viewMatrix);
		_projectorShader.setMat4Uniform("projectionMatrix", _camera.projMatrix);

		_light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
		glm::vec3 lightPosCamSpace = glm::vec3(_camera.viewMatrix * glm::vec4(_light.position, 1.0));

		_projectorShader.setVec3Uniform("light.pos", lightPosCamSpace); //копируем положение уже в системе виртуальной камеры
		_projectorShader.setVec3Uniform("light.La", _light.ambient);
		_projectorShader.setVec3Uniform("light.Ld", _light.diffuse);
		_projectorShader.setVec3Uniform("light.Ls", _light.specular);

		{
			glm::vec3 projPos = glm::vec3(glm::cos(_projPhi) * glm::cos(_projTheta), glm::sin(_projPhi) * glm::cos(_projTheta), glm::sin(_projTheta)) * _projR;

			_projCamera.viewMatrix = glm::lookAt(projPos, glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			_projCamera.projMatrix = glm::perspective(glm::radians(25.0f), 1.0f, 0.1f, 100.f);

			_projectorShader.setMat4Uniform("projViewMatrix", _projCamera.viewMatrix);
			_projectorShader.setMat4Uniform("projProjectionMatrix", _projCamera.projMatrix);

			glm::mat4 projScaleBiasMatrix = glm::scale(glm::translate(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5)), glm::vec3(0.5, 0.5, 0.5));
			_projectorShader.setMat4Uniform("projScaleBiasMatrix", projScaleBiasMatrix);
		}

		glActiveTexture(GL_TEXTURE0);  //текстурный юнит 0
		glBindTexture(GL_TEXTURE_2D, _brickTexId);
		glBindSampler(0, _sampler);
		_projectorShader.setIntUniform("diffuseTex", 0);

		glActiveTexture(GL_TEXTURE1);  //текстурный юнит 1
		glBindTexture(GL_TEXTURE_2D, _worldTexId);
		glBindSampler(1, _projSampler);
		_projectorShader.setIntUniform("projTex", 1);

		//Загружаем на видеокарту матрицы модели мешей и запускаем отрисовку
		{
			_projectorShader.setMat4Uniform("modelMatrix", cube.modelMatrix());
			_projectorShader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * cube.modelMatrix()))));

			cube.draw();
		}

		{
			_projectorShader.setMat4Uniform("modelMatrix", sphere.modelMatrix());
			_projectorShader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * sphere.modelMatrix()))));

			sphere.draw();
		}

		{
			_projectorShader.setMat4Uniform("modelMatrix", bunny.modelMatrix());
			_projectorShader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * bunny.modelMatrix()))));

			bunny.draw();
		}

		{
			_projectorShader.setMat4Uniform("modelMatrix", ground.modelMatrix());
			_projectorShader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * ground.modelMatrix()))));

			ground.draw();
		}

		//Рисуем маркеры для всех источников света		
		{
			_markerShader.use();

			_markerShader.setMat4Uniform("mvpMatrix", _camera.projMatrix * _camera.viewMatrix * glm::translate(glm::mat4(1.0f), _light.position));
            _markerShader.setVec4Uniform("color", glm::vec4(_light.diffuse, 1.0f));
			marker.draw();
		}

		//Рисуем маркер для проектора
		{
			_markerShader.use();

			glm::vec3 projPos = glm::vec3(glm::cos(_projPhi) * glm::cos(_projTheta), glm::sin(_projPhi) * glm::cos(_projTheta), glm::sin(_projTheta)) * _projR;

			_markerShader.setMat4Uniform("mvpMatrix", _camera.projMatrix * _camera.viewMatrix * glm::translate(glm::mat4(1.0f), projPos));
            _markerShader.setVec4Uniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
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