#version 330

const vec3 weights = vec3(0.27, 0.67, 0.06);

uniform sampler2D tex;

in vec2 interpTc; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
	vec3 texColor = texture(tex, interpTc).rgb;	

	//Яркость
	float lum = dot(weights, texColor);
	fragColor = vec4(lum, lum, lum, 1.0);
}
