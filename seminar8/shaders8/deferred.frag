#version 330

uniform sampler2D normalsTex;
uniform sampler2D diffuseTex;
uniform sampler2D depthTex;

uniform mat4 projMatrixInverse;

struct LightInfo
{
	vec3 pos; //положение источника света в системе координат ВИРТУАЛЬНОЙ КАМЕРЫ!
	vec3 La; //цвет и интенсивность окружающего света
	vec3 Ld; //цвет и интенсивность диффузного света
	vec3 Ls; //цвет и интенсивность бликового света
};
uniform LightInfo light;

in vec2 texCoord; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

const vec3 Ks = vec3(1.0, 1.0, 1.0); //Коэффициент бликового отражения
const float shininess = 128.0;

void main()
{	
	vec3 diffuseColor = texture(diffuseTex, texCoord).rgb;
	
	if (all(lessThan(diffuseColor, vec3(0.1))))
	{
		discard;
	}

	float depthColor = texture(depthTex, texCoord).r;
	vec3 depthCoords = vec3(texCoord, depthColor) * 2.0 - 1.0;
	vec4 posCamSpace = projMatrixInverse * vec4(depthCoords, 1.0);
	posCamSpace.xyz /= posCamSpace.w;
			
	vec3 normalColor = texture(normalsTex, texCoord).rgb;	
	vec3 normal = normalize(normalColor * 2.0 - 1.0);
		
	vec3 viewDirection = normalize(-posCamSpace.xyz); //направление на виртуальную камеру (она находится в точке (0.0, 0.0, 0.0))
	
	vec3 lightDirCamSpace = normalize(light.pos - posCamSpace.xyz); //направление на источник света	

	float NdotL = max(dot(normal, lightDirCamSpace.xyz), 0.0); //скалярное произведение (косинус)
	
	float distance = length(light.pos - posCamSpace.xyz);
	float attenuationCoef = 1.0 / (1.0 + 0.1 * distance);

	vec3 color = diffuseColor * (light.La + light.Ld * NdotL) * attenuationCoef;

	if (NdotL > 0.0)
	{			
		vec3 halfVector = normalize(lightDirCamSpace.xyz + viewDirection); //биссектриса между направлениями на камеру и на источник света

		float blinnTerm = max(dot(normal, halfVector), 0.0); //интенсивность бликового освещения по Блинну				
		blinnTerm = pow(blinnTerm, shininess); //регулируем размер блика
		color += light.Ls * Ks * blinnTerm * attenuationCoef;
	}

	fragColor = vec4(color, 1.0);
}
