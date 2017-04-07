#version 330

uniform sampler2D tex;

in vec2 texCoord; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
	vec3 texColor = texture(tex, texCoord).rgb;

	//Гамма-коррекция
	vec3 gamma = vec3(1.0 / 2.2);
	fragColor = vec4(pow(texColor, gamma), 1.0);
}
