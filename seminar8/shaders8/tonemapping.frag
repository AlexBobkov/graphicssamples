#version 330

uniform sampler2D tex; //основная текстура
uniform sampler2D bloomTex; //текстура с размытиыми яркими точками (больше заданного порога)

uniform float exposure; //параметр алгоритма Tone Mapping

in vec2 texCoord; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{	
	vec3 texColor = texture(tex, texCoord).rgb;
	vec3 bloomColor = texture(bloomTex, texCoord).rgb;

	vec3 color = texColor + bloomColor * 5.0;

	//Вариант 1 (с экспозицией)
	fragColor.rgb = vec3(1.0) - exp(-exposure * color); //tone mapping: HDR to LDR
	
	//Вариант 2 (Reinhard tone mapping)
	//fragColor.rgb = color / (color + vec3(1.0));	
	
	fragColor.a = 1.0;
}
