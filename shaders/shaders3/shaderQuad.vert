#version 330

uniform float time;

layout(location = 0) in vec3 vp;

out vec2 pos;

void main()
{
	pos = vec2(vp.x, vp.y);
	
	gl_Position = vec4(vp, 1.0);
}
