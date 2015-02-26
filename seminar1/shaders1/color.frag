/*
Простейший фрагментный шейдер для первого семинара. Подробности - в семинаре №3
*/

#version 330

in vec4 color;

out vec4 fragColor;

void main()
{
	fragColor = color;
}
