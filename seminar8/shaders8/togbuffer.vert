#version 330

//стандартные матрицы для преобразования координат
uniform mat4 modelMatrix; //из локальной в мировую
uniform mat4 viewMatrix; //из мировой в систему координат камеры
uniform mat4 projectionMatrix; //из системы координат камеры в усеченные координаты

//матрица для преобразования нормалей из локальной системы координат в систему координат камеры
uniform mat3 normalToCameraMatrix;

layout(location = 0) in vec3 vertexPosition; //координаты вершины в локальной системе координат
layout(location = 1) in vec3 vertexNormal; //нормаль в локальной системе координат
layout(location = 2) in vec2 vertexTexCoord; //текстурные координаты вершины

out vec3 normalCamSpace; //нормаль в системе координат камеры
out vec2 texCoord; //текстурные координаты

void main()
{
	texCoord = vertexTexCoord;	
	normalCamSpace = normalize(normalToCameraMatrix * vertexNormal); //преобразование нормали в систему координат камеры
	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}
