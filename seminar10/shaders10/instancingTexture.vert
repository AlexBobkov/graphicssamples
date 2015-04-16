#version 330

uniform samplerBuffer texBuf;

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
out vec4 posCamSpace; //координаты вершины в системе координат камеры
out vec2 texCoord; //текстурные координаты

void main()
{
	vec3 modelPos = texelFetch(texBuf, gl_InstanceID).rgb;

	texCoord = vertexTexCoord;

	posCamSpace = viewMatrix * modelMatrix * vec4(vertexPosition + modelPos, 1.0); //преобразование координат вершины в систему координат камеры
	normalCamSpace = normalize(normalToCameraMatrix * vertexNormal); //преобразование нормали в систему координат камеры
	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition + modelPos, 1.0);
}
