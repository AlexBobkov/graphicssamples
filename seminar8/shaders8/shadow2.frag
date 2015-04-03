#version 330

uniform sampler2D diffuseTex;
uniform sampler2DShadow shadowTex;

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

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

const float radiusInv = 700.0;

void main()
{	
	vec4 shadowCoords = shadowTexCoord;		
	shadowCoords.xyz /= shadowCoords.w;

	float visibility = 1.0;
	for (int i = 0; i < 4; i++)
	{		
		visibility -= 0.2 * (1.0 - texture(shadowTex, vec3(shadowCoords.xy + poissonDisk[i] / radiusInv, shadowCoords.z)));
	}
	
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
