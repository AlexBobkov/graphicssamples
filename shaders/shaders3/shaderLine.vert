#version 330

uniform float time;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

layout(location = 0) in float t;

out vec4 theColor;

void main()
{
	theColor = vec4(0.5 * (2.0 + sin(t)), 0.5 * (2.0 + cos(t)), 0.0, 1.0);
	
	vec4 pos = vec4(0.0, cos(t + time), sin(t), 1.0);
	
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * pos;
};