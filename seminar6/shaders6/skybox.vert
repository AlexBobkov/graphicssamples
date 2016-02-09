#version 330

uniform vec3 cameraPos; //положение виртуальной камеры в мировой системе координат
uniform mat4 viewMatrix; //из мировой в систему координат камеры
uniform mat4 projectionMatrix; //из системы координат камеры в усеченные координаты

layout(location = 0) in vec3 vertexPosition; //координаты вершины в локальной системе координат

out vec3 texCoord; //текстурные координаты

void main()
{
	texCoord.x = vertexPosition.x;
	texCoord.y = vertexPosition.y;
	texCoord.z = vertexPosition.z;	
	
	gl_Position = projectionMatrix * viewMatrix * vec4(vertexPosition + cameraPos, 1.0);
}
