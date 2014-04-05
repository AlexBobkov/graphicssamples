#version 330

uniform sampler2D tex;

in vec2 interpTc; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
	vec3 texColor = texture(tex, interpTc).rgb;

	fragColor = vec4(texColor, 1.0);

	//Осталяем только яркие фрагменты. Всё, что ниже порога, зануляем
	if (length(texColor) < 1.5)
	{
        fragColor.rgb = vec3(0.0);
	}
}
