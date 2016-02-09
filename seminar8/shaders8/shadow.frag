#version 330

uniform sampler2D diffuseTex;
//uniform sampler2D shadowTex; //Вариант 1
uniform sampler2DShadow shadowTex; //Вариант 2

struct LightInfo
{
	vec3 pos; //положение источника света в системе координат ВИРТУАЛЬНОЙ КАМЕРЫ!
	vec3 La; //цвет и интенсивность окружающего света
	vec3 Ld; //цвет и интенсивность диффузного света
	vec3 Ls; //цвет и интенсивность бликового света
};
uniform LightInfo light;

in vec3 normalCamSpace; //нормаль в системе координат камеры (интерполирована между вершинами треугольника)
in vec4 posCamSpace; //координаты вершины в системе координат камеры (интерполированы между вершинами треугольника)
in vec2 texCoord; //текстурные координаты (интерполирована между вершинами треугольника)
in vec4 shadowTexCoord; //выходные текстурные координаты для проективное текстуры

out vec4 fragColor; //выходной цвет фрагмента

const vec3 Ks = vec3(1.0, 1.0, 1.0); //Коэффициент бликового отражения
const float shininess = 128.0;

void main()
{
/*  Вариант 1: 
		
	vec4 shadowCoords = shadowTexCoord;		
	shadowCoords.xyz /= shadowCoords.w;

	float fragDepth = shadowCoords.z; //глубина фрагмента в пространстве источника света
	float shadowDepth = texture(shadowTex, shadowCoords.xy).r; //глубина ближайшего фрагмента в пространстве источника света

	float visibility = 1.0;
	if (fragDepth > shadowDepth)
	{
		visibility = 0.0;
	}
	
 */
	
/*  Вариант 2: */

	float visibility = textureProj(shadowTex, shadowTexCoord); //глубина ближайшего фрагмента в пространстве источника света
	
/**/
	
	//===============================
	
	vec3 diffuseColor = texture(diffuseTex, texCoord).rgb;

	vec3 normal = normalize(normalCamSpace); //нормализуем нормаль после интерполяции
	vec3 viewDirection = normalize(-posCamSpace.xyz); //направление на виртуальную камеру (она находится в точке (0.0, 0.0, 0.0))
	
	vec3 lightDirCamSpace = normalize(light.pos - posCamSpace.xyz); //направление на источник света	

	float NdotL = max(dot(normal, lightDirCamSpace.xyz), 0.0); //скалярное произведение (косинус)

	vec3 color = diffuseColor * (light.La + light.Ld * NdotL * visibility);

	if (NdotL > 0.0)
	{			
		vec3 halfVector = normalize(lightDirCamSpace.xyz + viewDirection); //биссектриса между направлениями на камеру и на источник света

		float blinnTerm = max(dot(normal, halfVector), 0.0); //интенсивность бликового освещения по Блинну				
		blinnTerm = pow(blinnTerm, shininess); //регулируем размер блика
		color += light.Ls * Ks * blinnTerm * visibility;
	}
	
	fragColor = vec4(color, 1.0);
}
