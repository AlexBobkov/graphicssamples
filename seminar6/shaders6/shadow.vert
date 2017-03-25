#version 330

//стандартные матрицы для преобразования координат
uniform mat4 modelMatrix; //из локальной в мировую
uniform mat4 viewMatrix; //из мировой в систему координат камеры
uniform mat4 projectionMatrix; //из системы координат камеры в усеченные координаты

//матрица для преобразования нормалей из локальной системы координат в систему координат камеры
uniform mat3 normalToCameraMatrix;

//стандартные матрицы для проектора
uniform mat4 lightViewMatrix; //из мировой в систему координат камеры
uniform mat4 lightProjectionMatrix; //из системы координат камеры в усеченные координаты
uniform mat4 lightScaleBiasMatrix;

layout(location = 0) in vec3 vertexPosition; //координаты вершины в локальной системе координат
layout(location = 1) in vec3 vertexNormal; //нормаль в локальной системе координат
layout(location = 2) in vec2 vertexTexCoord; //текстурные координаты вершины

out vec3 normalCamSpace; //нормаль в системе координат камеры
out vec4 posCamSpace; //координаты вершины в системе координат камеры
out vec2 texCoord; //текстурные координаты
out vec4 shadowTexCoord; //выходные текстурные координаты для проективное текстуры

void main()
{
	texCoord = vertexTexCoord;
	
	//вычисляем текстурные координаты для теневой карты
	shadowTexCoord = lightScaleBiasMatrix * lightProjectionMatrix * lightViewMatrix * modelMatrix * vec4(vertexPosition, 1.0);

	posCamSpace = viewMatrix * modelMatrix * vec4(vertexPosition, 1.0); //преобразование координат вершины в систему координат камеры
	normalCamSpace = normalize(normalToCameraMatrix * vertexNormal); //преобразование нормали в систему координат камеры
	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);	
}
