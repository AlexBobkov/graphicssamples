/*
Преобразует координаты вершины из локальной системы координат в Clip Space.
Копирует цвет вершины из вершинного атрибута в выходную переменную color.
Копирует координаты из вершинного атрибута в выходную переменную position
*/

#version 330

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexColor;

out vec4 color;
out vec4 position;

void main()
{
    color = vertexColor;
    position = vec4(vertexPosition, 1.0);

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}
