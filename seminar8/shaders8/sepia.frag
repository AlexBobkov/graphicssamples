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

	//Сепия
	vec3 sepiaColor = vec3(lum) * vec3(1.2, 1.0, 0.8);
    
    texColor = mix(texColor, sepiaColor, 0.75);
	
	fragColor = vec4(texColor, 1.0);
}
