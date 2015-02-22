/*
Простейший вершинный шейдер для первого семинара. Подробности - в семинаре №3
*/

#version 330

uniform mat4 projectionMatrix;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexColor;

out vec4 color;

void main()
{
	color = vertexColor;

	gl_Position = projectionMatrix * (vec4(vertexPosition, 1.0) + vec4(0.0, 0.0, -1.0, 0.0));
}
