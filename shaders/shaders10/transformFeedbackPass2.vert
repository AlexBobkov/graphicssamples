#version 330

//стандартные матрицы для преобразования координат
uniform mat4 modelMatrix; //из локальной в мировую
uniform mat4 viewMatrix; //из мировой в систему координат камеры
uniform mat4 projectionMatrix; //из системы координат камеры в усеченные координаты

layout(location = 0) in vec3 vp; //координаты вершины в локальной системе координат
layout(location = 1) in vec3 vel; 
layout(location = 2) in float particleTime;

out float ratio;

const float lifeTime = 3.0f;

void main()
{	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vp, 1.0);
	
	ratio = particleTime / lifeTime;

	gl_PointSize = 10.0 + ratio * 20.0;
}