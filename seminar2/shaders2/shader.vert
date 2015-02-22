/*
���������� ��������� ������ ��� ������� ��������. ����������� - � �������� �3
*/

#version 330

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

out vec3 color;

void main()
{
	/*
	��� �� ����� �������� ���� (�������� [0; 1]), �� ����� ������� ������� (�������� [-1; 1]).
	������� ����� ��������� � ���� ��� ���������� � ���������������� �����.
	*/
	color = (vertexNormal.xyz + 1.0) * 0.5;

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}
