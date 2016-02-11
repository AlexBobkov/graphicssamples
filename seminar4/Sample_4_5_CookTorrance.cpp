#include "Application.h"

#include <Mesh.hpp>
#include <ShaderProgram.hpp>

#include <iostream>
#include <vector>

/**
������ ��������� ����-��������
*/
class SampleApplication : public Application
{
public:
	Mesh cube;
	Mesh sphere;
	Mesh bunny;

	Mesh marker; //��� - ������ ��� ��������� �����

	//������������� ��������� ���������
	ShaderProgram _shader;
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
	float _roughnessValue;
	float _F0;

	virtual void makeScene()
	{
		Application::makeScene();

		//=========================================================
		//�������� � �������� �����		

		cube.makeCube(0.5);
		cube.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.5f));

		sphere.makeSphere(0.5, 100);
		sphere.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f));

		bunny.loadFromFile("models/bunny.obj");
		bunny.modelMatrix() = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		marker.makeSphere(0.1);

		//=========================================================
		//������������� ��������

		_shader.createProgram("shaders4/CookTorrance.vert", "shaders4/CookTorrance.frag");
		_markerShader.createProgram("shaders4/marker.vert", "shaders4/marker.frag");

		//=========================================================
		//������������� �������� ���������� ��������
		_lr = 5.0;
		_phi = 0.0;
		_theta = glm::pi<float>() * 0.25f;

		_lightAmbientColor = glm::vec3(0.0, 0.0, 0.0);
		_lightDiffuseColor = glm::vec3(0.0, 0.0, 0.0);
		_lightSpecularColor = glm::vec3(1.0, 1.0, 1.0);

		//������������� ��������� �������
		_rabbitAmbientColor = glm::vec3(1.0, 1.0, 0.0);
		_rabbitDiffuseColor = glm::vec3(1.0, 1.0, 0.0);
		_rabbitSpecularColor = glm::vec3(1.0, 1.0, 1.0);
		_roughnessValue = 0.3f;
		_F0 = 0.8f;
	}

	virtual void initGUI()
	{
		Application::initGUI();

		TwAddVarRW(_bar, "r", TW_TYPE_FLOAT, &_lr, "group=Light step=0.01 min=0.1 max=100.0");
		TwAddVarRW(_bar, "phi", TW_TYPE_FLOAT, &_phi, "group=Light step=0.01 min=0.0 max=6.28");
		TwAddVarRW(_bar, "theta", TW_TYPE_FLOAT, &_theta, "group=Light step=0.01 min=-1.57 max=1.57");
		TwAddVarRW(_bar, "La", TW_TYPE_COLOR3F, &_lightAmbientColor, "group=Light label='ambient'");
		TwAddVarRW(_bar, "Ld", TW_TYPE_COLOR3F, &_lightDiffuseColor, "group=Light label='diffuse'");
		TwAddVarRW(_bar, "Ls", TW_TYPE_COLOR3F, &_lightSpecularColor, "group=Light label='specular'");
		TwAddVarRW(_bar, "Ka", TW_TYPE_COLOR3F, &_rabbitAmbientColor, "group='Rabbit material' label='ambient'");
		TwAddVarRW(_bar, "Kd", TW_TYPE_COLOR3F, &_rabbitDiffuseColor, "group='Rabbit material' label='diffuse'");
		TwAddVarRW(_bar, "Ks", TW_TYPE_COLOR3F, &_rabbitSpecularColor, "group='Rabbit material' label='specular'");
		TwAddVarRW(_bar, "roughness", TW_TYPE_FLOAT, &_roughnessValue, "group='Rabbit material' step=0.01 min=0.1 max=1.0");
		TwAddVarRW(_bar, "F0", TW_TYPE_FLOAT, &_F0, "group='Rabbit material' step=0.01 min=0.1 max=1.0");
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

		//���������� ������		
		_shader.use();

		//��������� �� ���������� �������� �������-����������
		_shader.setMat4Uniform("viewMatrix", _viewMatrix);
		_shader.setMat4Uniform("projectionMatrix", _projMatrix);


		_shader.setVec3Uniform("light.pos", lightPos);
		_shader.setVec3Uniform("light.La", _lightAmbientColor);
		_shader.setVec3Uniform("light.Ld", _lightDiffuseColor);
		_shader.setVec3Uniform("light.Ls", _lightSpecularColor);

		//��������� �� ���������� ������� ������ ����� � ��������� ���������
		{
			_shader.setMat4Uniform("modelMatrix", cube.modelMatrix());
			_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * cube.modelMatrix()))));

			_shader.setVec3Uniform("material.Ka", glm::vec3(0.0, 1.0, 0.0));
			_shader.setVec3Uniform("material.Kd", glm::vec3(0.0, 1.0, 0.0));
			_shader.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
			_shader.setFloatUniform("material.roughnessValue", _roughnessValue);
			_shader.setFloatUniform("material.F0", _F0);

			cube.draw();
		}

		{
			_shader.setMat4Uniform("modelMatrix", sphere.modelMatrix());
			_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * sphere.modelMatrix()))));

			_shader.setVec3Uniform("material.Ka", glm::vec3(1.0, 1.0, 1.0));
			_shader.setVec3Uniform("material.Kd", glm::vec3(1.0, 1.0, 1.0));
			_shader.setVec3Uniform("material.Ks", glm::vec3(1.0, 1.0, 1.0));
			_shader.setFloatUniform("material.roughnessValue", _roughnessValue);
			_shader.setFloatUniform("material.F0", _F0);

			sphere.draw();
		}

		{
			_shader.setMat4Uniform("modelMatrix", bunny.modelMatrix());
			_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * bunny.modelMatrix()))));

			_shader.setVec3Uniform("material.Ka", glm::vec3(_rabbitAmbientColor));
			_shader.setVec3Uniform("material.Kd", glm::vec3(_rabbitDiffuseColor));
			_shader.setVec3Uniform("material.Ks", glm::vec3(_rabbitSpecularColor));
			_shader.setFloatUniform("material.roughnessValue", _roughnessValue);
			_shader.setFloatUniform("material.F0", _F0);

			bunny.draw();
		}

		//������ ������ ��� ��������� �����		
		{
			_markerShader.use();
			_markerShader.setMat4Uniform("mvpMatrix", _projMatrix * _viewMatrix * glm::translate(glm::mat4(1.0f), lightPos));
            _markerShader.setVec4Uniform("color", glm::vec4(_lightDiffuseColor, 1.0f));
			marker.draw();
		}
	}
};

int main()
{
	SampleApplication app;
	app.start();

	return 0;
}