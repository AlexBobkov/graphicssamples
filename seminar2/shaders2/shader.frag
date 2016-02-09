/*
Простейший фрагментный шейдер для первого семинара. Подробности - в семинаре №3
*/

#version 330

in vec3 color;

out vec4 fragColor;

void main()
{
	fragColor = vec4(color, 1.0);
}
