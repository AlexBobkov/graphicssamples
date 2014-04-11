#version 330

uniform sampler2D tex;

layout(location = 0) out vec4 fragColor; //выходной цвет фрагмента

void main()
{
	vec3 color = texture(tex, gl_PointCoord).rgb; //читаем из текстуры	

	fragColor = vec4(color, 1.0);
	//fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
