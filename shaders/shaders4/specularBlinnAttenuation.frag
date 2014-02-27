#version 330

uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininessFactor;
uniform vec3 material;
uniform float attenuation;

in vec3 normalCamSpace;
in vec4 lightPosCamSpace;
in vec4 posCamSpace;

out vec4 fragColor;

void main()
{	
	vec3 lightDirCamSpace = lightPosCamSpace.xyz - posCamSpace.xyz;
	float lightDistance = length(lightDirCamSpace);
	lightDirCamSpace = normalize(lightDirCamSpace);

	vec3 normal = normalize(normalCamSpace);
				    
    float cosAngIncidence = dot(normal, lightDirCamSpace);
    cosAngIncidence = clamp(cosAngIncidence, 0, 1);
    
	vec3 viewDirection = normalize(-posCamSpace.xyz);
		
	vec3 halfAngle = normalize(lightDirCamSpace + viewDirection);
	float blinnTerm = dot(normal, halfAngle);
	blinnTerm = clamp(blinnTerm, 0, 1);
	blinnTerm = cosAngIncidence != 0.0 ? blinnTerm : 0.0;
	blinnTerm = pow(blinnTerm, shininessFactor);

    vec3 color = material * (ambientColor + (diffuseColor * cosAngIncidence + specularColor * blinnTerm) / (1.0 + attenuation * lightDistance));	

	fragColor = vec4(color, 1.0);
}