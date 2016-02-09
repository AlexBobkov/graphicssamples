#version 330

uniform sampler2D tex;

in vec2 texCoord; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
	float depth = texture(tex, texCoord).r;	

	fragColor = vec4(depth, depth, depth, 1.0); //просто копируем	
}
