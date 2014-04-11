#version 330

uniform sampler2D tex;

layout(location = 0) out vec4 fragColor; //выходной цвет фрагмента

void main()
{
	vec4 color = texture(tex, gl_PointCoord); //читаем из текстуры	

	fragColor = color;
	//fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
