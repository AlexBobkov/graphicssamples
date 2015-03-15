#include "Application.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "Texture.h"

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
� ���������� ���������� ������������ ��������� �����������. ������ 1-6.
*/
class SampleApplication : public Application
{
public:
	Mesh cube;
	Mesh sphere;
	Mesh bunny;
	Mesh ground;

	Mesh marker; //��� - ������ ��� ��������� �����

	//������������� ��������� ���������
	ShaderProgram _shader;
	ShaderProgram _markerShader;

	//���������� ��� ���������� ���������� ������ ��������� �����
	float _lr;
	float _phi;
	float _theta;

	LightInfo _light;

	GLuint _worldTexId;
	GLuint _brickTexId;
	GLuint _grassTexId;
	GLuint _specularTexId;
	GLuint _chessTexId;
	GLuint _myTexId;

	GLuint _actualSampler;

	GLuint _samplerMinNearest;
	GLuint _samplerMinLinear;
	GLuint _samplerMinMipNearestNearest;
	GLuint _samplerMinMipLinearNearest;
	GLuint _samplerMinMipLinearLinear;
	GLuint _samplerMinMipLinearLinearAnisotropy;

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

		ground.makeGroundPlane(50.0f, 50.0f);

		marker.makeSphere(0.1);

		//=========================================================
		//������������� ��������

		_shader.createProgram("shaders5/texture.vert", "shaders5/texture.frag");
		_markerShader.createProgram("shaders5/marker.vert", "shaders5/marker.frag");

		//=========================================================
		//������������� �������� ���������� ��������
		_lr = 3.0;
		_phi = 0.0;
		_theta = glm::pi<float>() * 0.25f;

		_light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
		_light.ambient = glm::vec3(0.8);
		_light.diffuse = glm::vec3(0.2);
		_light.specular = glm::vec3(0.0);

		//=========================================================
		//�������� � �������� �������
		_worldTexId = Texture::loadTexture("images/earth_global.jpg");
		_brickTexId = Texture::loadTexture("images/brick.jpg");
		_grassTexId = Texture::loadTexture("images/grass.jpg");
		_specularTexId = Texture::loadTexture("images/specular.dds");
		_chessTexId = Texture::loadTextureWithMipmaps("images/chess.dds");
		_myTexId = Texture::makeProceduralTexture();

		//=========================================================
		//������������� ��������, �������, ������� ������ ��������� ������ �� ��������
		glGenSamplers(1, &_samplerMinNearest);
		glSamplerParameteri(_samplerMinNearest, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_samplerMinNearest, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glSamplerParameteri(_samplerMinNearest, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(_samplerMinNearest, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glGenSamplers(1, &_samplerMinLinear);
		glSamplerParameteri(_samplerMinLinear, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_samplerMinLinear, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glSamplerParameteri(_samplerMinLinear, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(_samplerMinLinear, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glGenSamplers(1, &_samplerMinMipNearestNearest);
		glSamplerParameteri(_samplerMinMipNearestNearest, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_samplerMinMipNearestNearest, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glSamplerParameteri(_samplerMinMipNearestNearest, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(_samplerMinMipNearestNearest, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glGenSamplers(1, &_samplerMinMipLinearNearest);
		glSamplerParameteri(_samplerMinMipLinearNearest, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_samplerMinMipLinearNearest, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glSamplerParameteri(_samplerMinMipLinearNearest, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(_samplerMinMipLinearNearest, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glGenSamplers(1, &_samplerMinMipLinearLinear);
		glSamplerParameteri(_samplerMinMipLinearLinear, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_samplerMinMipLinearLinear, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glSamplerParameteri(_samplerMinMipLinearLinear, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(_samplerMinMipLinearLinear, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glGenSamplers(1, &_samplerMinMipLinearLinearAnisotropy);
		glSamplerParameteri(_samplerMinMipLinearLinearAnisotropy, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glSamplerParameteri(_samplerMinMipLinearLinearAnisotropy, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glSamplerParameteri(_samplerMinMipLinearLinearAnisotropy, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glSamplerParameteri(_samplerMinMipLinearLinearAnisotropy, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glSamplerParameterf(_samplerMinMipLinearLinearAnisotropy, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);

		_actualSampler = _samplerMinNearest;

		//====
		GLfloat maxAniso = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);

		std::cout << "Max anistropy " << maxAniso << std::endl;
		//====
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
				_actualSampler = _samplerMinNearest;
			}
			else if (key == GLFW_KEY_2)
			{
				_actualSampler = _samplerMinLinear;
			}
			else if (key == GLFW_KEY_3)
			{
				_actualSampler = _samplerMinMipNearestNearest;
			}
			else if (key == GLFW_KEY_4)
			{
				_actualSampler = _samplerMinMipLinearNearest;
			}
			else if (key == GLFW_KEY_5)
			{
				_actualSampler = _samplerMinMipLinearLinear;
			}
			else if (key == GLFW_KEY_6)
			{
				_actualSampler = _samplerMinMipLinearLinearAnisotropy;
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//���������� ������		
		_shader.use();

		//��������� �� ���������� �������� �������-����������
		_shader.setMat4Uniform("viewMatrix", _viewMatrix);
		_shader.setMat4Uniform("projectionMatrix", _projMatrix);

		_light.position = glm::vec3(glm::cos(_phi) * glm::cos(_theta), glm::sin(_phi) * glm::cos(_theta), glm::sin(_theta)) * (float)_lr;
		glm::vec3 lightPosCamSpace = glm::vec3(_viewMatrix * glm::vec4(_light.position, 1.0));

		_shader.setVec3Uniform("light.pos", lightPosCamSpace); //�������� ��������� ��� � ������� ����������� ������
		_shader.setVec3Uniform("light.La", _light.ambient);
		_shader.setVec3Uniform("light.Ld", _light.diffuse);
		_shader.setVec3Uniform("light.Ls", _light.specular);

		glActiveTexture(GL_TEXTURE0);  //���������� ���� 0
		glBindTexture(GL_TEXTURE_2D, _chessTexId);
		glBindSampler(0, _actualSampler);
		_shader.setIntUniform("diffuseTex", 0);

		//��������� �� ���������� ������� ������ ����� � ��������� ���������
		//{
		//	_shader.setMat4Uniform("modelMatrix", cube.modelMatrix());
		//	_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * cube.modelMatrix()))));

		//	cube.draw();
		//}

		//{
		//	_shader.setMat4Uniform("modelMatrix", sphere.modelMatrix());
		//	_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * sphere.modelMatrix()))));

		//	sphere.draw();
		//}

		//{
		//	_shader.setMat4Uniform("modelMatrix", bunny.modelMatrix());
		//	_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * bunny.modelMatrix()))));

		//	bunny.draw();
		//}

		{
			_shader.setMat4Uniform("modelMatrix", ground.modelMatrix());
			_shader.setMat3Uniform("normalToCameraMatrix", glm::transpose(glm::inverse(glm::mat3(_viewMatrix * ground.modelMatrix()))));

			ground.draw();
		}

		//������ ������� ��� ���� ���������� �����		
		{
			_markerShader.use();

			_markerShader.setMat4Uniform("mvpMatrix", _projMatrix * _viewMatrix * glm::translate(glm::mat4(1.0f), _light.position));
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