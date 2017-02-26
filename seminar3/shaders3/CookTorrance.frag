/**
Пофрагментное освещение точечным источником света с учетом затухания. Окружающий, диффузный и бликовый свет.
Бликовое освещение считается по модели Кука-Торренса.
Код был заимствован отсюда: http://ruh.li/GraphicsCookTorrance.html
*/

#version 330

struct LightInfo
{
    vec3 pos; //положение источника света в мировой системе координат (для точечного источника)
    vec3 ambient;
    vec3 color;
};
uniform LightInfo light;

struct MaterialInfo
{
    vec3 Kd; //коэффициент отражения
    vec3 Ks;
    float diffuseFraction;
    float F0;
    float roughnessValue;
};
uniform MaterialInfo material;

in vec3 normalCamSpace; //нормаль в системе координат камеры (интерполирована между вершинами треугольника)
in vec4 lightPosCamSpace; //положение источника света в системе координат камеры (интерполировано между вершинами треугольника)
in vec4 posCamSpace; //координаты вершины в системе координат камеры (интерполированы между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

void main()
{
    vec3 lightDirCamSpace = normalize(lightPosCamSpace.xyz - posCamSpace.xyz); //направление на источник света

    vec3 normal = normalize(normalCamSpace); //нормализуем нормаль после интерполяции

    float NdotL = max(dot(normal, lightDirCamSpace.xyz), 0.0); //скалярное произведение (косинус)

    vec3 color = light.ambient * material.Kd + light.color * material.Kd * material.diffuseFraction * NdotL; //цвет вершины

    if (NdotL > 0.0)
    {
        vec3 viewDirection = normalize(-posCamSpace.xyz); //направление на виртуальную камеру (она находится в точке (0.0, 0.0, 0.0))

        // calculate intermediary values
        vec3 halfVector = normalize(lightDirCamSpace.xyz + viewDirection);
        float NdotH = max(dot(normal, halfVector), 0.0);
        float NdotV = max(dot(normal, viewDirection), 0.0); // note: this could also be NdotL, which is the same value
        float VdotH = max(dot(viewDirection, halfVector), 0.0);
        float mSquared = material.roughnessValue * material.roughnessValue;

        // geometric attenuation
        float NH2 = 2.0 * NdotH;
        float g1 = (NH2 * NdotV) / VdotH;
        float g2 = (NH2 * NdotL) / VdotH;
        float geoAtt = min(1.0, min(g1, g2));

        // roughness (or: microfacet distribution function)
        // beckmann distribution function
        float r1 = 1.0 / (3.14 * mSquared * pow(NdotH, 4.0));
        float r2 = (NdotH * NdotH - 1.0) / (mSquared * NdotH * NdotH);
        float roughness = r1 * exp(r2);

        // fresnel
        // Schlick approximation
        float fresnel = pow(1.0 - VdotH, 5.0);
        fresnel *= (1.0 - material.F0);
        fresnel += material.F0;

        color += light.color * material.Ks * (1.0 - material.diffuseFraction) * NdotL * (fresnel * geoAtt * roughness) / (NdotV * NdotL * 3.14);
    }

    fragColor = vec4(color, 1.0); //просто копируем
}
