#version 330

uniform sampler2D diffuseTex;

struct LightInfo
{
	vec3 pos; //положение источника света в системе координат ВИРТУАЛЬНОЙ КАМЕРЫ!
	vec3 La; //цвет и интенсивность окружающего света
	vec3 Ld; //цвет и интенсивность диффузного света
	vec3 Ls; //цвет и интенсивность бликового света
	float a0;
	float a1;
	float a2;
};
uniform LightInfo light;

in vec3 normalCamSpace; //нормаль в системе координат камеры (интерполирована между вершинами треугольника)
in vec4 posCamSpace; //координаты вершины в системе координат камеры (интерполированы между вершинами треугольника)
in vec2 texCoord; //текстурные координаты (интерполирована между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

const vec3 Ks = vec3(1.0, 1.0, 1.0); //Коэффициент бликового отражения
const float shininess = 128.0;

void main()
{
	vec3 diffuseColor = texture(diffuseTex, texCoord).rgb;
	
	vec3 lightDirCamSpace = light.pos - posCamSpace.xyz; //направление на источник света
	float distance = length(lightDirCamSpace);
	lightDirCamSpace = normalize(lightDirCamSpace); //направление на источник света

	vec3 normal = normalize(normalCamSpace); //нормализуем нормаль после интерполяции
	
	float attenuationCoef = 1.0 / (light.a0 + light.a1 * distance + light.a2 * distance * distance);
	
	float NdotL = max(dot(normal, lightDirCamSpace.xyz), 0.0); //скалярное произведение (косинус)

	vec3 color = diffuseColor * (light.La + light.Ld * NdotL) * attenuationCoef;

	if (NdotL > 0.0)
	{			
		vec3 viewDirection = normalize(-posCamSpace.xyz); //направление на виртуальную камеру (она находится в точке (0.0, 0.0, 0.0))
		vec3 halfVector = normalize(lightDirCamSpace.xyz + viewDirection); //биссектриса между направлениями на камеру и на источник света

		float blinnTerm = max(dot(normal, halfVector), 0.0); //интенсивность бликового освещения по Блинну				
		blinnTerm = pow(blinnTerm, shininess); //регулируем размер блика
		
		color += light.Ls * Ks * blinnTerm * attenuationCoef;
	}

	fragColor = vec4(color, 0.5);
}
