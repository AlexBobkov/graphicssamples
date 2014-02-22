#version 330

uniform mat4 projectionMatrix;

layout(location = 0) in vec3 vp;
layout(location = 1) in vec4 color;

out vec4 theColor;

void main()
{
	theColor = color;
	gl_Position = projectionMatrix * (vec4(vp, 1.0) + vec4(0.4, 0.4, -1.0, 0.0));
};