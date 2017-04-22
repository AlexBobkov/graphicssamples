#version 430 core

struct LightInfo
{
	vec3 pos; //положение источника света в мировой системе координат (для точечного источника)
	vec3 La; //цвет и интенсивность окружающего света
	vec3 Ld; //цвет и интенсивность диффузного света
	vec3 Ls; //цвет и интенсивность бликового света
};
uniform LightInfo light;

//in vec2 texCoords;
in vec3 normalCamSpace;
in vec3 posCamSpace;

out vec4 fragColor; //выходной цвет фрагмента

const vec3 Ka = vec3(1.0, 1.0, 1.0);
const vec3 Kd = vec3(1.0, 1.0, 1.0);
const vec3 Ks = vec3(1.0, 1.0, 1.0);
const float shininess = 128.0;

void main()
{
    vec3 normal = normalize(normalCamSpace); //нормализуем нормаль после интерполяции
	vec3 viewDirection = normalize(-posCamSpace.xyz); //направление на виртуальную камеру (она находится в точке (0.0, 0.0, 0.0))

    vec3 lightDirCamSpace = normalize(light.pos - posCamSpace); //направление на источник света	

    float NdotL = max(dot(normal, lightDirCamSpace), 0.0); //скалярное произведение (косинус)

    vec3 color = light.La * Ka + light.Ld * Kd * NdotL; //цвет вершины

    if (NdotL > 0.0)
    {			
        vec3 halfVector = normalize(lightDirCamSpace + viewDirection); //биссектриса между направлениями на камеру и на источник света

        float blinnTerm = max(dot(normal, halfVector), 0.0); //интенсивность бликового освещения по Блинну				
        blinnTerm = pow(blinnTerm, shininess); //регулируем размер блика

        color += light.Ls * Ks * blinnTerm;
    }	

	//fragColor = vec4(color, 1.0);
    fragColor = vec4(1.0);
}
