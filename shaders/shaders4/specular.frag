#version 330

uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininessFactor;

in vec3 normalCamSpace;
in vec4 lightPosCamSpace;
in vec4 posCamSpace;

out vec4 fragColor;

void main()
{
	vec3 lightDirCamSpace = normalize(lightPosCamSpace.xyz - posCamSpace.xyz);

	vec3 normal = normalize(normalCamSpace);
				    
    float cosAngIncidence = dot(normal, lightDirCamSpace);
    cosAngIncidence = clamp(cosAngIncidence, 0, 1);
    
	vec3 viewDirection = normalize(-posCamSpace.xyz);
	vec3 reflectDir = reflect(-lightDirCamSpace, normal);
	
	float phongTerm = dot(viewDirection, reflectDir);
	phongTerm = clamp(phongTerm, 0, 1);
	phongTerm = cosAngIncidence != 0.0 ? phongTerm : 0.0;
	phongTerm = pow(phongTerm, shininessFactor);

    vec3 color = ambientColor + diffuseColor * cosAngIncidence + specularColor * phongTerm;	

	fragColor = vec4(color, 1.0);
}