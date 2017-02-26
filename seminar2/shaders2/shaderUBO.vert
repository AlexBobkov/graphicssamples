/*
Преобразует координаты вершины из локальной системы координат в Clip Space.
Копирует цвет вершины из вершинного атрибута в выходную переменную color.
*/

#version 330

layout (std140) uniform Matrices
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

uniform mat4 modelMatrix;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexColor;

out vec4 color;

void main()
{
    color = vertexColor;

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}
