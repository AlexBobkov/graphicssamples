#version 330

uniform sampler2D tex;
uniform float exposure;

in vec2 interpTc; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
	vec3 texColor = texture(tex, interpTc).rgb;
	
	fragColor.rgb = vec3(1.0) - exp(-exposure * texColor.rgb);
    fragColor.a = 1.0;
}
