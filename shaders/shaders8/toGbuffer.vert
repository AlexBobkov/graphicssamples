#version 330

//стандартные матрицы для преобразования координат
uniform mat4 modelMatrix; //из локальной в мировую
uniform mat4 viewMatrix; //из мировой в систему координат камеры
uniform mat4 projectionMatrix; //из системы координат камеры в усеченные координаты

//матрица для преобразования нормалей из локальной системы координат в систему координат камеры
uniform mat3 normalToCameraMatrix;

layout(location = 0) in vec3 vp; //координаты вершины в локальной системе координат
layout(location = 1) in vec3 normal; //нормаль в локальной системе координат
layout(location = 2) in vec2 tc; //текстурные координаты

out vec3 normalCamSpace; //нормаль в системе координат камеры
out vec2 interpTc; //выходные текстурные координаты для диффузной текстуры

void main()
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vp, 1.0); //преобразование координат вершины в систему координат камеры;

	normalCamSpace = normalize(normalToCameraMatrix * normal); //преобразование нормали в систему координат камеры

	interpTc = tc; //просто копируем
};
