/*
Простейший вершинный шейдер для первого семинара. Подробности - в семинаре №3
*/

#version 130

uniform mat4 projectionMatrix;

in vec3 vertexPosition;
in vec4 vertexColor;

out vec4 color;

void main()
{
	color = vertexColor;

	gl_Position = projectionMatrix * (vec4(vertexPosition, 1.0) + vec4(0.0, 0.0, -1.0, 0.0));
}
