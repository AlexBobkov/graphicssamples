#version 330

uniform sampler2D tex;

in vec2 texCoord; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
	vec3 texColor = texture(tex, texCoord).rgb;	

	//Яркость
	const vec3 weights = vec3(0.27, 0.67, 0.06);
	float lum = dot(texColor, weights);
	
	fragColor = vec4(lum, lum, lum, 1.0);
}
