/*
Простейший вершинный шейдер для первого семинара. Подробности - в семинаре №2
*/

#version 330

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

out vec4 color;

void main()
{
    /*
    Меш не имеет атрибута цвет (диапазон [0; 1]), но имеет атрибут нормаль (диапазон [-1; 1]).
    Нормаль легко перевести в цвет для отладочных и демонстрационных задач.
    */
    color.rgb = (vertexNormal.xyz + 1.0) * 0.5;
    color.a = 1.0;

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}
