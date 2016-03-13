/**
Простой шейдер для отрисовки маркера источника света
*/

#version 330

uniform mat4 mvpMatrix; //произведение матриц projectionMatrix * viewMatrix * modelMatrix

layout(location = 0) in vec3 vertexPosition; //координаты вершины в локальной системе координат

void main()
{	
	gl_Position = mvpMatrix * vec4(vertexPosition, 1.0);
}
