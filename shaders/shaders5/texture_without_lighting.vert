#version 330

//стандартные матрицы для преобразования координат
uniform mat4 modelMatrix; //из локальной в мировую
uniform mat4 viewMatrix; //из мировой в систему координат камеры
uniform mat4 projectionMatrix; //из системы координат камеры в усеченные координаты

layout(location = 0) in vec3 vp; //координаты вершины в локальной системе координат
layout(location = 1) in vec3 normal; //нормаль в локальной системе координат
layout(location = 2) in vec2 tc; //текстурные координаты

out vec2 interpTc; //выходные текстурные координаты

void main()
{	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vp, 1.0);

	interpTc = tc;
}
