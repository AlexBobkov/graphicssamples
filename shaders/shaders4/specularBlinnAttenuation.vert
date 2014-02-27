#version 330

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalToCameraMatrix;

uniform vec4 lightPos;

layout(location = 0) in vec3 vp;
layout(location = 1) in vec3 normal;

out vec3 normalCamSpace;
out vec4 lightPosCamSpace;
out vec4 posCamSpace;

void main()
{
	posCamSpace = viewMatrix * modelMatrix * vec4(vp, 1.0);
	gl_Position = projectionMatrix * posCamSpace;

	normalCamSpace = normalize(normalToCameraMatrix * normal);
	lightPosCamSpace = viewMatrix * lightPos;
};