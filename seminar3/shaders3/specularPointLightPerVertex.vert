/**
Повершинное освещение точечным источником света. Окружающий, диффузный и бликовый свет.
*/

#version 330

//стандартные матрицы для преобразования координат
uniform mat4 modelMatrix; //из локальной в мировую
uniform mat4 viewMatrix; //из мировой в систему координат камеры
uniform mat4 projectionMatrix; //из системы координат камеры в усеченные координаты

//матрица для преобразования нормалей из локальной системы координат в систему координат камеры
uniform mat3 normalToCameraMatrix;

struct LightInfo
{
    vec3 pos; //положение источника света в мировой системе координат (для точечного источника)
    vec3 La; //цвет и интенсивность окружающего света
    vec3 Ld; //цвет и интенсивность диффузного света
    vec3 Ls; //цвет и интенсивность бликового света
};
uniform LightInfo light;

struct MaterialInfo
{
    vec3 Ka; //коэффициент отражения окружающего света
    vec3 Kd; //коэффициент отражения диффузного света
    vec3 Ks; //коэффициент отражения бликового света
    float shininess;
};
uniform MaterialInfo material;

layout(location = 0) in vec3 vertexPosition; //координаты вершины в локальной системе координат
layout(location = 1) in vec3 vertexNormal; //нормаль в локальной системе координат

out vec3 color; //выходной цвет вершины

void main()
{
    vec3 normalCamSpace = normalize(normalToCameraMatrix * vertexNormal); //нормаль - из локальной в систему координат камеры

    vec4 posCamSpace = viewMatrix * modelMatrix * vec4(vertexPosition, 1.0); //координаты вершины из локальной в систему координат камеры
    vec4 lightPosCamSpace = viewMatrix * vec4(light.pos, 1.0); //положение источника света - из мировой в систему координат камеры
    vec4 lightDirCamSpace = normalize(lightPosCamSpace - posCamSpace);

    float NdotL = max(dot(normalCamSpace, lightDirCamSpace.xyz), 0.0); //скалярное произведение (косинус)

    color = light.La * material.Ka + light.Ld * material.Kd * NdotL; //цвет вершины

    if (NdotL > 0.0)
    {
        vec3 viewDirection = normalize(-posCamSpace.xyz); //направление на виртуальную камеру (она находится в точке (0.0, 0.0, 0.0))
        vec3 halfVector = normalize(lightDirCamSpace.xyz + viewDirection); //биссектриса между направлениями на камеру и на источник света

        float blinnTerm = max(dot(normalCamSpace, halfVector), 0.0); //интенсивность бликового освещения по Блинну
        blinnTerm = pow(blinnTerm, material.shininess); //регулируем размер блика

        color += light.Ls * material.Ks * blinnTerm;
    }

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}
