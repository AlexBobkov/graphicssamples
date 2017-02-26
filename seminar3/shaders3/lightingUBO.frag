/**
Пофрагментное освещение точечным источником света с учетом затухания. Окружающий, диффузный и бликовый свет.
*/

#version 330

layout(std140) uniform LightInfo
{
    vec3 pos; //положение источника света в мировой системе координат (для точечного источника)
    vec3 La; //цвет и интенсивность переотраженного света
    vec3 Ld; //цвет и интенсивность диффузного света
    vec3 Ls; //цвет и интенсивность бликового света
    float a0; //коэффициент затухания
    float a1; //коэффициент затухания
    float a2; //коэффициент затухания
} light;

layout(std140) uniform MaterialInfo
{
    vec3 Ka; //коэффициент отражения окружающего света
    vec3 Kd; //коэффициент отражения диффузного света
    vec3 Ks; //коэффициент отражения бликового света
    float shininess;
} material;

in vec3 normalCamSpace; //нормаль в системе координат камеры (интерполирована между вершинами треугольника)
in vec4 posCamSpace; //координаты вершины в системе координат камеры (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
    vec3 normal = normalize(normalCamSpace); //нормализуем нормаль после интерполяции

    vec3 lightDirCamSpace = normalize(light.pos - posCamSpace.xyz); //направление на источник света

    float distance = length(light.pos - posCamSpace.xyz);
    float attenuation = 1.0 / (light.a0 + light.a1 * distance + light.a2 * distance * distance);

    float NdotL = max(dot(normal, lightDirCamSpace), 0.0); //скалярное произведение (косинус)

    vec3 color = light.La * material.Ka + light.Ld * material.Kd * NdotL * attenuation; //цвет вершины

    if (NdotL > 0.0)
    {
        vec3 viewDirection = normalize(-posCamSpace.xyz); //направление на виртуальную камеру (она находится в точке (0.0, 0.0, 0.0))
        vec3 halfVector = normalize(lightDirCamSpace + viewDirection); //биссектриса между направлениями на камеру и на источник света

        float blinnTerm = max(dot(normal, halfVector), 0.0); //интенсивность бликового освещения по Блинну
        blinnTerm = pow(blinnTerm, material.shininess); //регулируем размер блика

        color += light.Ls * material.Ks * blinnTerm * attenuation;
    }

    fragColor = vec4(color, 1.0); //просто копируем
}
