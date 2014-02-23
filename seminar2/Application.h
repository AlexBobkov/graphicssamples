#pragma

#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

class Application
{
public:
	Application();
	~Application();

	//������������� ������������ ���������
	void initContext();

	//��������� ��������� ���������� OpenGL
	void initGL();

	//�������� ���������� �����
	void makeScene();

	//���� ����������
	void run();

	//���������� ���� ����
	void draw();

	//����������
	void update();

	void rotateLeft(bool b) { _rotateLeft = b; }
	void rotateRight(bool b) { _rotateRight = b; }

	void rotateUp(bool b) { _rotateUp = b; }
	void rotateDown(bool b) { _rotateDown = b; }

	void fovInc(bool b) { _fovInc = b; }
	void fovDec(bool b) { _fovDec = b; }
		
protected:	
	GLFWwindow* _window;
	GLuint _vao;
	GLuint _shaderProgram;

	GLuint _modelMatrixUniform;
	GLuint _viewMatrixUniform;
	GLuint _projMatrixUniform;
	
	glm::mat4 _modelMatrix;
	glm::mat4 _viewMatrix;
	glm::mat4 _projMatrix;
	
	double _oldTime;

	bool _rotateLeft;
	bool _rotateRight;
	double _phiAng;

	bool _rotateUp;
	bool _rotateDown;
	double _thetaAng;

	double _z;

	bool _fovInc;
	bool _fovDec;
	float _fov;

	//������ ����� ������� �� ����� � ������� ������
	GLuint createShader(GLenum shaderType, std::string filename);

	//���������� ���������� ��� ������ �������� �� �������� (�����������, ���)
	void makeSceneImplementation();

	void drawImplementation();
};