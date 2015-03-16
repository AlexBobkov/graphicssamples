#version 330

uniform vec3 cameraPos; //положение виртуальной камеры в мировой системе координат
uniform mat4 viewMatrix; //из мировой в систему координат камеры
uniform mat4 projectionMatrix; //из системы координат камеры в усеченные координаты

layout(location = 0) in vec3 vp; //координаты вершины в локальной системе координат
layout(location = 1) in vec3 normal; //нормаль в локальной системе координат
layout(location = 2) in vec2 tc; //текстурные координаты

out vec3 interpTc; //выходные текстурные координаты

void main()
{	
	gl_Position = projectionMatrix * viewMatrix * vec4(vp + cameraPos, 1.0);

	//fix coords from world space to camera space
	interpTc.x = vp.x;
	interpTc.y = -vp.z;
	interpTc.z = vp.y;	
}
