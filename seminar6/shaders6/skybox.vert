#version 330

uniform vec3 cameraPos; //положение виртуальной камеры в мировой системе координат
uniform mat4 viewMatrix; //из мировой в систему координат камеры
uniform mat4 projectionMatrix; //из системы координат камеры в усеченные координаты

uniform mat3 textureMatrix; //матрица для превращения координат из локальной системы кординат в текстурные координаты

layout(location = 0) in vec3 vertexPosition; //координаты вершины в локальной системе координат

out vec3 texCoord; //текстурные координаты

void main()
{
	texCoord = textureMatrix * vertexPosition;
	
	gl_Position = projectionMatrix * viewMatrix * vec4(vertexPosition + cameraPos, 1.0);
}
