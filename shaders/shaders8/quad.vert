#version 330

layout(location = 0) in vec3 vp; //координаты вершины в локальной системе координат

out vec2 interpTc; //выходные текстурные координаты

void main()
{	
	gl_Position = vec4(vp, 1.0);

	interpTc = (vp.xy + 1.0) * 0.5;
}
