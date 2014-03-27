#version 330

uniform vec3 color;

layout(location = 0) out vec3 fragColor; //выходной цвет фрагмента

void main()
{
	fragColor = color * 10; //в переменную color для удобства передаем diffuse color, а он меньше 1, поэтому немного усилим
}
