#version 330

uniform sampler2D diffuseTex;
//uniform sampler2D shadowTex;
uniform sampler2DShadow shadowTex;

uniform vec3 ambientColor; //цвет окружающего света (аппроксимация множественных переотражений)
uniform vec3 diffuseColor; //цвет источника света
uniform vec3 specularColor; //бликовый цвет источника света
uniform float shininessFactor; //блеск (свойство материала, влияет на размер блика)

in vec3 normalCamSpace; //нормаль в системе координат камеры (интерполирована между вершинами треугольника)
in vec4 lightPosCamSpace; //положение источника света в системе координат камеры (интерполировано между вершинами треугольника)
in vec4 posCamSpace; //координаты вершины в системе координат камеры (интерполированы между вершинами треугольника)

in vec2 interpTc;
in vec4 interShadowTc;

out vec4 fragColor; //выходной цвет фрагмента

const float bias = 0.00005;

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

void main()
{
	vec3 diffuseMaterial = texture(diffuseTex, interpTc).rgb; //читаем из текстуры
	
	vec4 shadowCoords = interShadowTc;		
	shadowCoords.xyz /= shadowCoords.w;

	//float fragDepth = shadowCoords.z; //глубина фрагмента в пространстве источника света
	//float shadowDepth = texture(shadowTex, shadowCoords.xy).z; //глубина ближайшего фрагмента в пространстве источника света	

	//float visibility = 1.0;
	//if (fragDepth > shadowDepth + bias)
	//{
	//	visibility = 0.5;
	//}

	//float visibility = texture(shadowTex, vec3(shadowCoords.xy, shadowCoords.z - bias));

	float visibility = 1.0;
	for (int i = 0; i < 4; i++)
	{		
		visibility -= 0.2 * (1.0 - texture(shadowTex, vec3(shadowCoords.xy + poissonDisk[i] / 700.0, shadowCoords.z-bias)));
	}

	vec3 lightDirCamSpace = lightPosCamSpace.xyz - posCamSpace.xyz; //направление на источник света
	lightDirCamSpace = normalize(lightDirCamSpace); //нормализуем направление

	vec3 normal = normalize(normalCamSpace); //нормализуем нормаль после интерполяции
				    
    float cosAngIncidence = dot(normal, lightDirCamSpace); //интенсивность диффузного света
    cosAngIncidence = clamp(cosAngIncidence, 0, 1);
    
	vec3 viewDirection = normalize(-posCamSpace.xyz); //направление на виртуальную камеру (она находится в точке (0.0, 0.0, 0.0))
		
	vec3 halfAngle = normalize(lightDirCamSpace + viewDirection); //биссектриса между направлениями на камеру и на источник света
	float blinnTerm = dot(normal, halfAngle); //интенсивность бликового освещения по Блинну
	blinnTerm = clamp(blinnTerm, 0, 1);
	blinnTerm = cosAngIncidence != 0.0 ? blinnTerm : 0.0;
	blinnTerm = pow(blinnTerm, shininessFactor);  //регулируем размер блика

	//результирующий цвет
    vec3 color = diffuseMaterial * ambientColor + visibility * diffuseMaterial * diffuseColor * cosAngIncidence + visibility * specularColor * blinnTerm;
		
	fragColor = vec4(color, 1.0);
}
