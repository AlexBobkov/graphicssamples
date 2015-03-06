#include "Application.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#include <iostream>
#include <vector>

/**
�������� �������� �����
*/
class SampleApplication : public Application
{
public:
	MeshPtr cube;
	MeshPtr sphere;
	MeshPtr bunny;

	MeshPtr marker; //��� - ������ ��� ��������� �����

	//������������� ��������� ���������
	ShaderProgram _shaderPerVertex;
	ShaderProgram _shaderPerFragment;
	ShaderProgram _markerShader;

	float _lr;
	float _phi;
	float _theta;

	//����������, ������� �������� ��������, ������� ����� �������� � uniform-���������� ��������
	glm::vec3 _lightAmbientColor;
	glm::vec3 _lightDiffuseColor;
	glm::vec3 _lightSpecularColor;

	glm::vec3 _rabbitAmbientColor;
	glm::vec3 _rabbitDiffuseColor;
	glm::vec3 _rabbitSpecularColor;
	float _rabbitShininess;

	virtual void makeScene()
	{
		Application::makeScene();

		//=========================================================
		//�������� � �������� �����		

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
		//������������� ��������

		_shaderPerVertex.createProgram("shaders4/specularPointLightPerVertex.vert", "shaders4/specularPointLightPerVertex.frag");
		_shaderPerFragment.createProgram("shaders4/specularPointLightPerFragment.vert", "shaders4/specularPointLightPerFragment.frag");
		_markerShader.createProgram("shaders4/marker.vert", "shaders4/marker.frag");

		//=========================================================
		//������������� �������� ���������� ��������
		_lr = 5.0;
		_phi = 0.0;
		_theta = glm::pi<float>() * 0.25f;

		_lightAmbientColor = glm::vec3(0.2, 0.2, 0.2);
		_lightDiffuseColor = glm::vec3(0.8, 0.8, 0.8);
		_lightSpecularColor = glm::vec3(1.0, 1.0, 1.0);

		//������������� ��������� �������
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
		TwAddVarRW(_bar, "Ka", TW_TYPE_COLOR3F, &_rabbitAmbientColor, " group='Rabbit material' label='ambient' ");
		TwAddVarRW(_bar, "Kd", TW_TYPE_COLOR3F, &_rabbitDiffuseColor, " group='Rabbit material' label='diffuse' ");
		TwAddVarRW(_bar, "Ks", TW_TYPE_COLOR3F, &_rabbitSpecularColor, " group='Rabbit material' label='specular' ");
		TwAddVarRW(_bar, "shininess", TW_TYPE_FLOAT, &_rabbitShininess, " group='Rabbit material' min=0.1 max=255.0");
	}

	virtual void draw()
	{
		//�������� ������� ������� ������ � ��������� �������
		int width, height;
		glfwGetFramebufferSize(_window, &width, &height);

		glViewport(0, 0, width, height);

		//������� ������ ����� � ������� �� ����������� ���������� ����������� �����
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec3 lightPos = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * _lr;

		//������� ��������� ������ � ����������� ����������
		{
			//���������� ������		
			_shaderPerVertex.use();

			//��������� �� ���������� �������� �������-����������
			_shaderPerVertex.setMat4Uniform("viewMatrix", _viewMatrix);
			_shaderPerVertex.setMat4Uniform("projectionMatrix", _projMatrix);


			_shaderPerVertex.setVec3Uniform("light.pos", lightPos);
			_shaderPerVertex.setVec3Uniform("light.La", _lightAmbientColor);
			_shaderPerVertex.setVec3Uniform("light.Ld", _lightDiffuseColor);
			_shaderPerVertex.setVec3Uniform("light.Ls", _lightSpecularColor);

			//��������� �� ���������� ������� ������ ����� � ��������� ���������
			{
				_shaderPerVertex.setMat4Uniform("modelMatrix", cube->modelMatrix());
				_shaderPerVertex.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * cube->modelMatrix()))));

				_shaderPerVertex.setVec3Uniform("material.Ka", glm::vec3(0.0, 1.0, 0.0));
				_shaderPerVertex.setVec3Uniform("material.Kd", glm::vec3(0.0, 1.0, 0.0));
				_shaderPerVertex.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
				_shaderPerVertex.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
				_shaderPerVertex.setFloatUniform("material.shininess", _rabbitShininess);

				cube->draw();
			}

			{
				_shaderPerVertex.setMat4Uniform("modelMatrix", sphere->modelMatrix());
				_shaderPerVertex.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * sphere->modelMatrix()))));

				_shaderPerVertex.setVec3Uniform("material.Ka", glm::vec3(1.0, 1.0, 1.0));
				_shaderPerVertex.setVec3Uniform("material.Kd", glm::vec3(1.0, 1.0, 1.0));
				_shaderPerVertex.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
				_shaderPerVertex.setFloatUniform("material.shininess", _rabbitShininess);

				sphere->draw();
			}

			{
				_shaderPerVertex.setMat4Uniform("modelMatrix", bunny->modelMatrix());
				_shaderPerVertex.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * bunny->modelMatrix()))));

				_shaderPerVertex.setVec3Uniform("material.Ka", glm::vec3(_rabbitAmbientColor));
				_shaderPerVertex.setVec3Uniform("material.Kd", glm::vec3(_rabbitDiffuseColor));
				_shaderPerVertex.setVec3Uniform("material.Ks", glm::vec3(_rabbitSpecularColor));
				_shaderPerVertex.setFloatUniform("material.shininess", _rabbitShininess);

				bunny->draw();
			}
		}

		//������� ��������� ������ � ������������� ����������
		{
			//���������� ������		
			_shaderPerFragment.use();

			//��������� �� ���������� �������� �������-����������
			_shaderPerFragment.setMat4Uniform("viewMatrix", _viewMatrix);
			_shaderPerFragment.setMat4Uniform("projectionMatrix", _projMatrix);


			_shaderPerFragment.setVec3Uniform("light.pos", lightPos);
			_shaderPerFragment.setVec3Uniform("light.La", _lightAmbientColor);
			_shaderPerFragment.setVec3Uniform("light.Ld", _lightDiffuseColor);
			_shaderPerFragment.setVec3Uniform("light.Ls", _lightSpecularColor);

			//��������� �� ���������� ������� ������ ����� � ��������� ���������
			{
				_shaderPerFragment.setMat4Uniform("modelMatrix", glm::translate(cube->modelMatrix(), glm::vec3(0.0f, 0.0f, 1.5f)));
				_shaderPerFragment.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * cube->modelMatrix()))));

				_shaderPerFragment.setVec3Uniform("material.Ka", glm::vec3(0.0, 1.0, 0.0));
				_shaderPerFragment.setVec3Uniform("material.Kd", glm::vec3(0.0, 1.0, 0.0));
				_shaderPerFragment.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
				_shaderPerFragment.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
				_shaderPerFragment.setFloatUniform("material.shininess", _rabbitShininess);

				cube->draw();
			}

			{
				_shaderPerFragment.setMat4Uniform("modelMatrix", glm::translate(sphere->modelMatrix(), glm::vec3(0.0f, 0.0f, 1.5f)));
				_shaderPerFragment.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * sphere->modelMatrix()))));

				_shaderPerFragment.setVec3Uniform("material.Ka", glm::vec3(1.0, 1.0, 1.0));
				_shaderPerFragment.setVec3Uniform("material.Kd", glm::vec3(1.0, 1.0, 1.0));
				_shaderPerFragment.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
				_shaderPerFragment.setFloatUniform("material.shininess", _rabbitShininess);

				sphere->draw();
			}

			{
				_shaderPerFragment.setMat4Uniform("modelMatrix", glm::translate(bunny->modelMatrix(), glm::vec3(0.0f, 0.0f, 1.5f)));
				_shaderPerFragment.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * bunny->modelMatrix()))));

				_shaderPerFragment.setVec3Uniform("material.Ka", glm::vec3(_rabbitAmbientColor));
				_shaderPerFragment.setVec3Uniform("material.Kd", glm::vec3(_rabbitDiffuseColor));
				_shaderPerFragment.setVec3Uniform("material.Ks", glm::vec3(_rabbitSpecularColor));
				_shaderPerFragment.setFloatUniform("material.shininess", _rabbitShininess);

				bunny->draw();
			}
		}

		//������ ������ ��� ��������� �����		
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