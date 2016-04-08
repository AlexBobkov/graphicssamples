#version 330

//стандартные матрицы для преобразования координат
uniform mat4 modelMatrix; //из локальной в мировую
uniform mat4 lightViewMatrix; //из мировой в систему координат камеры
uniform mat4 lightProjectionMatrix; //из системы координат камеры в усеченные координаты

layout(location = 0) in vec3 vertexPosition; //координаты вершины в локальной системе координат

void main()
{
	gl_Position = lightProjectionMatrix * lightViewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}
