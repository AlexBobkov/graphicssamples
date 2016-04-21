#version 330

uniform sampler2D tex;

layout(location = 0) out vec4 fragColor; //выходной цвет фрагмента

in float ratio;
in vec2 texCoord;

void main()
{
	vec4 color = texture(tex, texCoord); //читаем из текстуры	
	color.a *= (1.0 - ratio);

	fragColor = color;
}
