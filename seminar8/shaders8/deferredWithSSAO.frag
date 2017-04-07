#version 330

uniform sampler2D normalsTex;
uniform sampler2D diffuseTex;
uniform sampler2D depthTex;
uniform sampler2DShadow shadowTex;
uniform sampler2D ssaoTex;

uniform mat4 viewMatrixInverse;
uniform mat4 projMatrixInverse;

struct LightInfo
{
	vec3 pos; //положение источника света в системе координат ВИРТУАЛЬНОЙ КАМЕРЫ!
	vec3 La; //цвет и интенсивность окружающего света
	vec3 Ld; //цвет и интенсивность диффузного света
	vec3 Ls; //цвет и интенсивность бликового света
};
uniform LightInfo light;

uniform mat4 lightViewMatrix; //из мировой в систему координат камеры
uniform mat4 lightProjectionMatrix; //из системы координат камеры в усеченные координаты
uniform mat4 lightScaleBiasMatrix;

in vec2 texCoord; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

const vec3 Ks = vec3(1.0, 1.0, 1.0); //Коэффициент бликового отражения
const float shininess = 128.0;

void main()
{	
	vec3 diffuseColor = texture(diffuseTex, texCoord).rgb;
	
	if (all(equal(diffuseColor, vec3(0.0))))
	{
		discard;
	}
	
	//-------------------------------

	float fragDepth = texture(depthTex, texCoord).r;
	vec3 depthCoords = vec3(texCoord, fragDepth) * 2.0 - 1.0;
	vec4 posCamSpace = projMatrixInverse * vec4(depthCoords, 1.0);	
	posCamSpace.xyz /= posCamSpace.w;
	
	//-------------------------------
	
	vec4 posWorldSpace = viewMatrixInverse * projMatrixInverse * vec4(depthCoords, 1.0);
	vec4 shadowTexCoord = lightScaleBiasMatrix * lightProjectionMatrix * lightViewMatrix * posWorldSpace;
	float visibility = textureProj(shadowTex, shadowTexCoord); //глубина ближайшего фрагмента в пространстве источника света
	
	//-------------------------------
			
	vec3 normalColor = texture(normalsTex, texCoord).rgb;	
	vec3 normal = normalize(normalColor * 2.0 - 1.0);
	
	//-------------------------------
	
	vec3 lightDirCamSpace = normalize(light.pos - posCamSpace.xyz); //направление на источник света	

	float NdotL = max(dot(normal, lightDirCamSpace.xyz), 0.0); //скалярное произведение (косинус)
	
	vec3 ambientSSAO = texture(ssaoTex, texCoord).rgb; //модифицируем ambient-компоненты освещения

	vec3 color = diffuseColor * (light.La * ambientSSAO + light.Ld * NdotL * visibility);

	if (NdotL > 0.0)
	{			
		vec3 viewDirection = normalize(-posCamSpace.xyz); //направление на виртуальную камеру (она находится в точке (0.0, 0.0, 0.0))
		vec3 halfVector = normalize(lightDirCamSpace.xyz + viewDirection); //биссектриса между направлениями на камеру и на источник света

		float blinnTerm = max(dot(normal, halfVector), 0.0); //интенсивность бликового освещения по Блинну				
		blinnTerm = pow(blinnTerm, shininess); //регулируем размер блика
		
		color += light.Ls * Ks * blinnTerm * visibility;
	}

	fragColor = vec4(color, 1.0);
}
