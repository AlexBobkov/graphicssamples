#version 330

uniform float time;

//стандартные матрицы для преобразования координат
uniform mat4 modelMatrix; //из локальной в мировую
uniform mat4 viewMatrix; //из мировой в систему координат камеры
uniform mat4 projectionMatrix; //из системы координат камеры в усеченные координаты

layout(location = 0) in vec3 vertexPosition; //координаты вершины в локальной системе координат
layout(location = 1) in float startTime;

out float ratio;

const float lifeTime = 5.0f;

void main()
{	
	vec4 posCamSpace = viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
	
	//За время жизни частицы меняется от 0 до 1
	ratio = (time - startTime) / lifeTime;
	
	//Вычисляем размер частицы с учётом эффекта перспективы и времени жизни
	gl_PointSize = (100.0 + ratio * 100.0) / (-posCamSpace.z);
	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}
