/*
Получает на вход интеполированный цвет фрагмента и применяет к нему сдвиг на основе времени и y-координаты фрагмента
*/

#version 330

uniform float time;

in vec4 color;

out vec4 fragColor;

void main()
{
    vec4 colorOffset = vec4(0.5 * (1.0 + sin(time)), cos(gl_FragCoord.y), 0.0, 0.0);

    fragColor = color + colorOffset;
}
