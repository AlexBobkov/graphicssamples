#version 330

uniform float time;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout(location = 0) in vec3 vp;
layout(location = 1) in vec4 color;

out vec4 theColor;

void main()
{
	theColor = color;
	
	vec3 offset = vec3(sin(time), cos(time), 0.0);
			
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vp + offset, 1.0);
};