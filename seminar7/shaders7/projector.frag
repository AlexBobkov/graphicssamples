#version 330

uniform sampler2D diffuseTex;
uniform sampler2D projTex;

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
in vec4 projTexCoord; //выходные текстурные координаты для проективное текстуры

out vec4 fragColor; //выходной цвет фрагмента

const vec3 Ks = vec3(1.0, 1.0, 1.0); //Коэффициент бликового отражения
const float shininess = 128.0;

void main()
{
	vec3 diffuseColor = texture(diffuseTex, texCoord).rgb;

	vec3 normal = normalize(normalCamSpace); //нормализуем нормаль после интерполяции
	vec3 viewDirection = normalize(-posCamSpace.xyz); //направление на виртуальную камеру (она находится в точке (0.0, 0.0, 0.0))
	
	vec3 lightDirCamSpace = normalize(light.pos - posCamSpace.xyz); //направление на источник света	

	float NdotL = max(dot(normal, lightDirCamSpace.xyz), 0.0); //скалярное произведение (косинус)

	vec3 color = diffuseColor * (light.La + light.Ld * NdotL);

	if (NdotL > 0.0)
	{			
		vec3 halfVector = normalize(lightDirCamSpace.xyz + viewDirection); //биссектриса между направлениями на камеру и на источник света

		float blinnTerm = max(dot(normal, halfVector), 0.0); //интенсивность бликового освещения по Блинну				
		blinnTerm = pow(blinnTerm, shininess); //регулируем размер блика
		color += light.Ls * Ks * blinnTerm;
	}
	
	//==========================================
	
	//первый вариант: вручную выполняем перспективное деление
	vec4 projTc = projTexCoord;		
	projTc.xyz /= projTc.w;
	vec3 projColor = texture(projTex, projTc.xy).rgb; //читаем из текстуры

	//второй вариант: используем функцию textureProj
	//vec3 projColor = textureProj(projTex, projTexCoord).rgb; //читаем из текстуры
	
	color += projColor;
	
	//==========================================

	fragColor = vec4(color, 1.0);

/*
	vec3 diffuseMaterial = texture(diffuseTex, interpTc).rgb; //читаем из текстуры
	
	//первый вариант: вручную выполняем перспективное деление
	vec4 projTc = interProjTc;		
	projTc.xyz /= projTc.w;
	vec3 projColor = texture(projTex, projTc.xy).rgb; //читаем из текстуры

	//второй вариант: используем функцию textureProj
	//vec3 projColor = textureProj(projTex, interProjTc).rgb; //читаем из текстуры

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
    vec3 color = diffuseMaterial * ambientColor + diffuseMaterial * diffuseColor * cosAngIncidence + specularColor * blinnTerm;
	
	color += projColor;

	fragColor = vec4(color, 1.0);
	//fragColor = vec4(projColor, 1.0);
	*/
}
