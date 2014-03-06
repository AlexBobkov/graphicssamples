#version 330

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout(location = 0) in vec3 vp;
layout(location = 1) in vec4 color;

out vec4 interpColor;

void main()
{
	interpColor = color;
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vp, 1.0);
};