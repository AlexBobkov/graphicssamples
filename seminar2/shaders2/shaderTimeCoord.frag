/*
Получает на вход интеполированные цвет и координаты фрагмента и использует их для вычисления цвета.
*/

#version 330

uniform float time;

in vec4 color;
in vec4 position;

out vec4 fragColor;

void main()
{
    fragColor = position * 0.5 * (2.0 + sin(time));
}
