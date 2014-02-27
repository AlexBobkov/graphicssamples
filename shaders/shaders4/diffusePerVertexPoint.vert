#version 330

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalToCameraMatrix;

uniform vec4 lightPos;
uniform vec3 ambientColor;
uniform vec3 diffuseColor;

layout(location = 0) in vec3 vp;
layout(location = 1) in vec3 normal;

out vec3 interpColor;

void main()
{
	vec4 posCamSpace = viewMatrix * modelMatrix * vec4(vp, 1.0);
	gl_Position = projectionMatrix * posCamSpace;

	vec3 normalCamSpace = normalize(normalToCameraMatrix * normal);
	vec4 lightPosCamSpace = viewMatrix * lightPos;

	vec3 lightDirCamSpace = normalize(lightPosCamSpace.xyz - posCamSpace.xyz);
				    
    float cosAngIncidence = dot(normalCamSpace, lightDirCamSpace.xyz);
    cosAngIncidence = clamp(cosAngIncidence, 0, 1);
    
    interpColor = ambientColor + diffuseColor * cosAngIncidence;
};