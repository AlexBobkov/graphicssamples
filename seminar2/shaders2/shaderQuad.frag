/*
Рисует желтый круг на квадрате путём отбрасывания фрагментов дальше заданного радиуса
*/

#version 330

in vec2 position;

out vec4 fragColor;

void main()
{
    fragColor = vec4(1.0, 1.0, 0.0, 1.0);

    if (length(position) > 0.5)
    {
        discard;
    }
}
