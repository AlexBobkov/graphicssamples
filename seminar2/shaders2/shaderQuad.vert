/*
Принимает координаты сразу в Clip Space и копирует их в выходную переменную position
*/

#version 330

layout(location = 0) in vec3 vertexPosition;

out vec2 position;

void main()
{
    position = vertexPosition.xy;

    gl_Position = vec4(vertexPosition, 1.0);
}
