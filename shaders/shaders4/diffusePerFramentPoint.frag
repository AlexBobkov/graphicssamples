#version 330

uniform vec3 ambientColor;
uniform vec3 diffuseColor;

in vec3 normalCamSpace;
in vec4 lightPosCamSpace;
in vec4 posCamSpace;

out vec4 fragColor;

void main()
{
	vec3 lightDirCamSpace = normalize(lightPosCamSpace.xyz - posCamSpace.xyz);
				    
    float cosAngIncidence = dot(normalize(normalCamSpace), lightDirCamSpace.xyz);
    cosAngIncidence = clamp(cosAngIncidence, 0, 1);
    
    vec3 color = ambientColor + diffuseColor * cosAngIncidence;	

	fragColor = vec4(color, 1.0);
}