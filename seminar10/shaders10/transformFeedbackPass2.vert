#version 330

//стандартные матрицы для преобразования координат
uniform mat4 modelMatrix; //из локальной в мировую
uniform mat4 viewMatrix; //из мировой в систему координат камеры
uniform mat4 projectionMatrix; //из системы координат камеры в усеченные координаты

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexVelocity;
layout(location = 2) in float oldParticleTime;

out float ratio;

const float lifeTime = 15.0f;

void main()
{	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
			
	ratio = oldParticleTime / lifeTime;

	gl_PointSize = 10.0 + ratio * 20.0;
}
