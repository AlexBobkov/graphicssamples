#pragma

#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>

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
		
protected:	
	GLFWwindow* _window;
	GLuint _vao;
	GLuint _shaderProgram;
	GLuint _projMatrixUniform;
	float _projMatrix[16];

	//������ ����� ������� �� ����� � ������� ������
	GLuint createShader(GLenum shaderType, std::string filename);

	//���������� ���������� ��� ������ �������� �� �������� (�����������, ���)
	void makeSceneImplementation();
	void makeSceneImplementation2();
	void makeSceneImplementation3();
	void makeSceneImplementation4();

	void drawImplementation();
	void drawImplementation3();
	void drawImplementation4();
};