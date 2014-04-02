#version 330

uniform sampler2D normalsTex;
uniform sampler2D diffuseTex;
uniform sampler2D depthTex;
//uniform sampler2DShadow shadowTex;
uniform sampler2D shadowTex;

uniform mat4 projMatrixInverse;
uniform mat4 viewMatrixInverse;

//стандартные матрицы для проектора
uniform mat4 lightViewMatrix; //из мировой в систему координат камеры
uniform mat4 lightProjectionMatrix; //из системы координат камеры в усеченные координаты
uniform mat4 lightScaleBiasMatrix;

uniform vec3 ambientColor; //цвет окружающего света (аппроксимация множественных переотражений)
uniform vec3 diffuseColor; //цвет источника света
uniform vec3 specularColor; //бликовый цвет источника света

const float shininessFactor = 200.0; //блеск (свойство материала, влияет на размер блика)

in vec4 lightPosCamSpace; //положение источника света в системе координат камеры (интерполировано между вершинами треугольника)
in vec2 interpTc; //текстурные координаты (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

vec2 poissonDisk[4] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 )
);

const float bias = 0.0005;

void main()
{	
	vec3 diffuseMaterial = texture(diffuseTex, interpTc).rgb;

	vec3 normalColor = texture(normalsTex, interpTc).rgb;	
	vec3 normal = normalColor * 2.0 - 1.0;

	if (length(normalColor) < 0.1)
	{
		discard;
		return;
	}
	
	vec3 depthColor = texture(depthTex, interpTc).rgb;
	vec3 normCoords = vec3(interpTc * 2.0 - 1.0, depthColor.z * 2.0 - 1.0);
	vec4 pos = projMatrixInverse * vec4(normCoords, 1.0);
	pos.xyz /= pos.w;


	//вычисляем текстурные координаты для теневой карты
	vec4 shadowTc = lightScaleBiasMatrix * lightProjectionMatrix * lightViewMatrix * viewMatrixInverse * vec4(pos.xyz, 1.0);	
	shadowTc.xyz /= shadowTc.w;

	//float visibility = 1.0;
	//for (int i = 0; i < 4; i++)
	//{		
	//	visibility -= 0.2 * (1.0 - texture(shadowTex, vec3(shadowTc.xy + poissonDisk[i] / 700.0, shadowTc.z - bias)));
	//}

	float fragDepth = shadowTc.z; //глубина фрагмента в пространстве источника света
	float shadowDepth = texture(shadowTex, shadowTc.xy).z; //глубина ближайшего фрагмента в пространстве источника света

	float visibility = 1.0;
	if (fragDepth > shadowDepth)
	{
		visibility = 0.5;
	}


	vec3 lightDirCamSpace = lightPosCamSpace.xyz - pos.xyz; //направление на источник света
	lightDirCamSpace = normalize(lightDirCamSpace); //нормализуем направление
			    
    float cosAngIncidence = dot(normal, lightDirCamSpace); //интенсивность диффузного света
    cosAngIncidence = clamp(cosAngIncidence, 0, 1);

	vec3 viewDirection = normalize(-pos.xyz); //направление на виртуальную камеру (она находится в точке (0.0, 0.0, 0.0))
		
	vec3 halfAngle = normalize(lightDirCamSpace + viewDirection); //биссектриса между направлениями на камеру и на источник света
	float blinnTerm = dot(normal, halfAngle); //интенсивность бликового освещения по Блинну
	blinnTerm = clamp(blinnTerm, 0, 1);
	blinnTerm = cosAngIncidence != 0.0 ? blinnTerm : 0.0;
	blinnTerm = pow(blinnTerm, shininessFactor);  //регулируем размер блика

	//результирующий цвет
    vec3 color = diffuseMaterial * ambientColor + visibility * diffuseMaterial * diffuseColor * cosAngIncidence + visibility * specularColor * blinnTerm;

	fragColor = vec4(color, 1.0);
	
	//fragColor = vec4(diffuseMaterial, 1.0);
	//fragColor = vec4(normalColor, 1.0);
	//fragColor = vec4(depthColor, 1.0);
	//fragColor = vec4(texture(shadowTex, shadowTc.xy).rgb, 1.0);
	//fragColor = vec4(interpTc, 0.0, 1.0);
	//fragColor = vec4(shadowTc.xy, 0.0, 1.0);
	//fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
