/*
Применяет к координатам вращение и преобразует их в Clip Space.
Копирует цвет вершины из вершинного атрибута в выходную переменную color.
*/

#version 330

uniform float time;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexColor;

out vec4 color;

void main()
{
    color = vertexColor;

    vec3 offset = vec3(sin(time), cos(time), 0.0); //Сдвиг локальных координат

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition + offset, 1.0);
}
