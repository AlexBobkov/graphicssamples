/**
Пофрагментное освещение несколькими источниками света
*/

#version 330

struct LightInfo
{
    vec3 pos; //положение источника света в системе координат ВИРТУАЛЬНОЙ КАМЕРЫ!
    vec3 La; //цвет и интенсивность окружающего света
    vec3 Ld; //цвет и интенсивность диффузного света
    vec3 Ls; //цвет и интенсивность бликового света
};
uniform LightInfo light[3];

struct MaterialInfo
{
    vec3 Ka; //коэффициент отражения окружающего света
    vec3 Kd; //коэффициент отражения диффузного света
    vec3 Ks; //коэффициент отражения бликового света
    float shininess;
};
uniform MaterialInfo material;

in vec3 normalCamSpace; //нормаль в системе координат камеры (интерполирована между вершинами треугольника)
in vec4 posCamSpace; //координаты вершины в системе координат камеры (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
    vec3 normal = normalize(normalCamSpace); //нормализуем нормаль после интерполяции
    vec3 viewDirection = normalize(-posCamSpace.xyz); //направление на виртуальную камеру (она находится в точке (0.0, 0.0, 0.0))

    vec3 color = vec3(0.0);

    for (int i = 0; i < 3; i++)
    {
        vec3 lightDirCamSpace = normalize(light[i].pos - posCamSpace.xyz); //направление на источник света

        float NdotL = max(dot(normal, lightDirCamSpace.xyz), 0.0); //скалярное произведение (косинус)

        color += light[i].La * material.Ka + light[i].Ld * material.Kd * NdotL; //цвет вершины

        if (NdotL > 0.0)
        {
            vec3 halfVector = normalize(lightDirCamSpace.xyz + viewDirection); //биссектриса между направлениями на камеру и на источник света

            float blinnTerm = max(dot(normal, halfVector), 0.0); //интенсивность бликового освещения по Блинну
            blinnTerm = pow(blinnTerm, material.shininess); //регулируем размер блика

            color += light[i].Ls * material.Ks * blinnTerm;
        }
    }

    fragColor = vec4(color, 1.0); //просто копируем
}
