#version 330

in vec3 interpColor; //интерполированный цвет между вершинами треугольника

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
	fragColor = vec4(interpColor, 1.0); //просто копируем
}
