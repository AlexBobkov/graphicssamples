/*
Простейший вершинный шейдер для первого семинара. Подробности - в семинаре №3
*/

#version 130

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

in vec3 vertexPosition;
in vec3 vertexNormal;

out vec3 color;

void main()
{
	/*
	Меш не имеет атрибута цвет (диапазон [0; 1]), но имеет атрибут нормаль (диапазон [-1; 1]).
	Нормаль легко перевести в цвет для отладочных и демонстрационных задач.
	*/
	color = (vertexNormal.xyz + 1.0) * 0.5;

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}
