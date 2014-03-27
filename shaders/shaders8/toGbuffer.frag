#version 330

uniform sampler2D diffuseTex;

in vec3 normalCamSpace; //нормаль в системе координат камеры (интерполирована между вершинами треугольника)
in vec2 interpTc;

layout(location = 0) out vec3 normal; // "go to GL_COLOR_ATTACHMENT0"
layout(location = 1) out vec3 diffuseColor; // "go to GL_COLOR_ATTACHMENT1"

void main()
{
	diffuseColor = texture(diffuseTex, interpTc).rgb; //читаем из текстуры
	normal = normalize(normalCamSpace) * 0.5 + 0.5;				    
}
