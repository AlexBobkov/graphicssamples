#version 330

uniform samplerCube cubeTex;

in vec3 texCoord; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
	vec3 texColor = texture(cubeTex, texCoord).rgb;

	fragColor = vec4(texColor, 1.0);
}
