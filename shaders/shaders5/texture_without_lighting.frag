#version 330

uniform sampler2D diffuseTex;

in vec2 interpTc; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
	vec3 texColor = texture(diffuseTex, interpTc).rgb;

	fragColor = vec4(texColor, 1.0); //просто копируем
	//fragColor = vec4(interpTc, 0.0, 1.0); //просто копируем
}