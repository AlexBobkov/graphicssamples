#version 330

uniform mat4 projectionMatrix;

layout(location = 0) in vec3 vp;

void main()
{
	gl_Position = projectionMatrix * (vec4(vp, 1.0) + vec4(0.0, 0.0, -1.0, 0.0));
};