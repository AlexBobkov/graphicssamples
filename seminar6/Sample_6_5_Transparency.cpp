#include "Application.h"

#include <Mesh.hpp>
#include <ShaderProgram.hpp>
#include <Texture.hpp>

#include <iostream>
#include <sstream>
#include <vector>

struct LightInfo
{
	glm::vec3 position; //����� ����� ������� ���������� � ������� ������� ���������, � ��� ����������� � �������-���������� �������������� � ������� ����������� ������
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

/**
������ � �������������
*/
class SampleApplication : public Application
{
public:
	Mesh cube;
	Mesh sphere;
	Mesh bunny;
	Mesh plane;
	Mesh backgroundCube;

	Mesh marker; //��� - ������ ��� ��������� �����

	//������������� ��������� ���������
	ShaderProgram _commonShader;
	ShaderProgram _markerShader;
	ShaderProgram _skyboxShader;

	//���������� ��� ���������� ���������� ������ ��������� �����
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

	bool backFaceCullEnabled;
	bool blendEnabled;
	bool depthTestEnabled;

	virtual void makeScene()
	{
		Application::makeScene();

		backFaceCullEnabled = false;
		blendEnabled = false;
		depthTestEnabled = true;

		//=========================================================
		//�������� � �������� �����		

		cube.makeCube(0.5);
		cube.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f));

		sphere.makeSphere(0.5, 100);
		sphere.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f));

		bunny.loadFromFile("models/bunny.obj");
		bunny.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		plane.makeGroundPlane(1.0f, 1.0f);

		marker.makeSphere(0.1);

		backgroundCube.makeCube(10.0f);

		//=========================================================
		//������������� ��������

		_commonShader.createProgram("shaders6/common.vert", "shaders6/common.frag");
		_markerShader.createProgram("shaders4/marker.vert", "shaders4/marker.frag");
		_skyboxShader.createProgram("shaders6/skybox.vert", "shaders6/skybox.frag");

		//=========================================================
		//������������� �������� ���������� ��������
		_lr = 10.0;
		_phi = 0.0f;
		_theta = 0.48f;

		_light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
		_light.ambient = glm::vec3(0.2, 0.2, 0.2);
		_light.diffuse = glm::vec3(0.8, 0.8, 0.8);
		_light.specular = glm::vec3(1.0, 1.0, 1.0);

		//=========================================================
		//�������� � �������� �������
		_worldTexId = Texture::loadTexture("images/earth_global.jpg");
		_brickTexId = Texture::loadTexture("images/brick.jpg");
		_grassTexId = Texture::loadTexture("images/grass.jpg");
		_chessTexId = Texture::loadTextureWithMipmaps("images/chess.dds");
		_myTexId = Texture::makeProceduralTexture();
		_cubeTexId = Texture::loadCubeTexture("images/cube");

		//=========================================================
		//������������� ��������, �������, ������� ������ ��������� ������ �� ��������
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

	virtual void handleKey(int key, int scancode, int action, int mods)
	{
		Application::handleKey(key, scancode, action, mods);

		if (action == GLFW_PRESS)
		{
			if (key == GLFW_KEY_1)
			{
				backFaceCullEnabled = !backFaceCullEnabled;
			}
			else if (key == GLFW_KEY_2)
			{
				blendEnabled = !blendEnabled;
			}
			else if (key == GLFW_KEY_3)
			{
				depthTestEnabled = !depthTestEnabled;
			}
		}
	}

	virtual void draw()
	{
		//�������� ������� ������� ������ � ��������� �������
		int width, height;
		glfwGetFramebufferSize(_window, &width, &height);

		glViewport(0, 0, width, height);

		//������� ������ ����� � ������� �� ����������� ���������� ����������� �����
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//====== ������ �������� ������� ����� ======
		_commonShader.use();

		//��������� �� ���������� �������� �������-����������
		_commonShader.setMat4Uniform("viewMatrix", _camera.viewMatrix);
		_commonShader.setMat4Uniform("projectionMatrix", _camera.projMatrix);

		_light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
		glm::vec3 lightPosCamSpace = glm::vec3(_camera.viewMatrix * glm::vec4(_light.position, 1.0));

		_commonShader.setVec3Uniform("light.pos", lightPosCamSpace); //�������� ��������� ��� � ������� ����������� ������
		_commonShader.setVec3Uniform("light.La", _light.ambient);
		_commonShader.setVec3Uniform("light.Ld", _light.diffuse);
		_commonShader.setVec3Uniform("light.Ls", _light.specular);
		
		if (backFaceCullEnabled)
		{
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CW);
			glCullFace(GL_BACK);
		}

		if (blendEnabled)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		if (!depthTestEnabled)
		{
			glDisable(GL_DEPTH_TEST);
		}

		glActiveTexture(GL_TEXTURE0);  //���������� ���� 0
		glBindTexture(GL_TEXTURE_2D, _grassTexId);
		glBindSampler(0, _sampler);
		_commonShader.setIntUniform("diffuseTex", 0);

		//��������� �� ���������� ������� ������ ����� � ��������� ���������
		{
			_commonShader.setMat4Uniform("modelMatrix", cube.modelMatrix());
			_commonShader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * cube.modelMatrix()))));

			cube.draw();
		}

		glActiveTexture(GL_TEXTURE0);  //���������� ���� 0
		glBindTexture(GL_TEXTURE_2D, _worldTexId);
		glBindSampler(0, _sampler);
		_commonShader.setIntUniform("diffuseTex", 0);

		{
			_commonShader.setMat4Uniform("modelMatrix", sphere.modelMatrix());
			_commonShader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * sphere.modelMatrix()))));

			sphere.draw();
		}

		glFrontFace(GL_CCW); //bunny has another front face orientation :(

		glActiveTexture(GL_TEXTURE0);  //���������� ���� 0
		glBindTexture(GL_TEXTURE_2D, _brickTexId);
		glBindSampler(0, _sampler);
		_commonShader.setIntUniform("diffuseTex", 0);

		{
			_commonShader.setMat4Uniform("modelMatrix", bunny.modelMatrix());
			_commonShader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_camera.viewMatrix * bunny.modelMatrix()))));

			bunny.draw();
		}

		if (!depthTestEnabled)
		{
			glEnable(GL_DEPTH_TEST);
		}

		if (blendEnabled)
		{
			glDisable(GL_BLEND);
		}

		if (backFaceCullEnabled)
		{
			glDisable(GL_CULL_FACE);
		}

		//������ ������� ��� ���� ���������� �����		
		{
			_markerShader.use();

			_markerShader.setMat4Uniform("mvpMatrix", _camera.projMatrix * _camera.viewMatrix * glm::translate(glm::mat4(1.0f), _light.position));
            _markerShader.setVec4Uniform("color", glm::vec4(_light.diffuse, 1.0f));
			marker.draw();
		}

		//����������� ������� � ��������� ���������
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