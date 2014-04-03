#version 330

const vec3 luminance = vec3(0.3, 0.59, 0.11);

uniform sampler2D tex;
uniform float exposure;

in vec2 interpTc; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
	vec3 texColor = texture(tex, interpTc).rgb;

	fragColor.rgb = vec3(1.0) - exp(-exposure * texColor.rgb);

	//float l = dot(luminance, texColor);
	//float scale = l / (1.0 + l);
	//fragColor.rgb = texColor * scale;	

	fragColor.a = 1.0;
}
