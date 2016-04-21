#version 330

uniform sampler2D tex;

layout(location = 0) out vec4 fragColor; //выходной цвет фрагмента

in float ratio;

void main()
{
	vec4 color = texture(tex, gl_PointCoord); //читаем из текстуры	
	color.a *= (1.0 - ratio);

	fragColor = color;
}
