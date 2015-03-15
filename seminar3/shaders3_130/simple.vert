/*
Простейший вершинный шейдер. Не выполняет никаких действий.
Предполагается, что входной вершинный атрибут содержит координаты уже в Clip Space
*/

#version 130

in vec3 vertexPosition;

void main()
{
	gl_Position = vec4(vertexPosition, 1.0);
}
