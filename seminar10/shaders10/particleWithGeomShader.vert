#version 330

uniform float time;

//стандартные матрицы для преобразования координат
uniform mat4 modelMatrix; //из локальной в мировую
uniform mat4 viewMatrix; //из мировой в систему координат камеры
uniform mat4 projectionMatrix; //из системы координат камеры в усеченные координаты

layout(location = 0) in vec3 vertexPosition; //координаты вершины в локальной системе координат
layout(location = 1) in float startTime;

out float ratioVert;

const float lifeTime = 5.0f;

void main()
{
	ratioVert = (time - startTime) / lifeTime;
	
	gl_Position = viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}
