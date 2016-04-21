#version 330

uniform sampler2D tex;

layout(location = 0) out vec4 fragColor; //выходной цвет фрагмента

in vec2 texCoord; //текстурные координаты

void main()
{
	fragColor = texture(tex, texCoord);
}
