#version 330

layout(location = 0) in vec3 vertexPosition; //координаты вершины в локальной системе координат

out vec2 texCoord; //выходные текстурные координаты

void main()
{	
	texCoord = (vertexPosition.xy + 1.0) * 0.5;
	
	gl_Position = vec4(vertexPosition, 1.0);	
}
