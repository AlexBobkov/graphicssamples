#version 330

//����������� ������� ��� �������������� ���������
uniform mat4 modelMatrix; //�� ��������� � �������
uniform mat4 viewMatrix; //�� ������� � ������� ��������� ������
uniform mat4 projectionMatrix; //�� ������� ��������� ������ � ��������� ����������

layout(location = 0) in vec3 vp; //���������� ������� � ��������� ������� ���������
layout(location = 1) in vec3 normal; //������� � ��������� ������� ���������
layout(location = 2) in vec2 tc; //���������� ����������

out vec2 interpTc; //�������� ���������� ����������

void main()
{	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vp, 1.0);

	interpTc = tc;
};