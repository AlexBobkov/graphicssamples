#version 330

uniform sampler2D tex;

in vec2 texCoord; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
	vec3 color = texture(tex, texCoord).rgb;	

	fragColor = vec4(color, 1.0); //просто копируем	
}
