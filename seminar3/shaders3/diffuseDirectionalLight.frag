#version 330

in vec3 color; //интерполированный цвет между вершинами треугольника

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
    fragColor = vec4(color, 1.0); //просто копируем
}
