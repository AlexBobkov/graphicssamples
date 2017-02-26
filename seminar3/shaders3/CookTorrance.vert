/**
Пофрагментное освещение точечным источником света с учетом затухания. Окружающий, диффузный и бликовый свет.
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
    vec3 ambient;
    vec3 color;
};
uniform LightInfo light;

layout(location = 0) in vec3 vertexPosition; //координаты вершины в локальной системе координат
layout(location = 1) in vec3 vertexNormal; //нормаль в локальной системе координат

out vec3 normalCamSpace; //нормаль в системе координат камеры
out vec4 lightPosCamSpace; //положение источника света в системе координат камеры
out vec4 posCamSpace; //координаты вершины в системе координат камеры

void main()
{
    posCamSpace = viewMatrix * modelMatrix * vec4(vertexPosition, 1.0); //преобразование координат вершины в систему координат камеры
    normalCamSpace = normalize(normalToCameraMatrix * vertexNormal); //преобразование нормали в систему координат камеры
    lightPosCamSpace = viewMatrix * vec4(light.pos, 1.0); //преобразование положения источника света в систему координат камеры

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}
