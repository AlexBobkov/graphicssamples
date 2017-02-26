/**
Пофрагментное освещение точечным источником света с учетом затухания. Окружающий, диффузный и бликовый свет.
*/

#version 330

uniform mat4 modelMatrix;
uniform mat3 normalToCameraMatrix;

layout(std140) uniform Matrices
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
};

layout(location = 0) in vec3 vertexPosition; //координаты вершины в локальной системе координат
layout(location = 1) in vec3 vertexNormal; //нормаль в локальной системе координат

out vec3 normalCamSpace; //нормаль в системе координат камеры
out vec4 posCamSpace; //координаты вершины в системе координат камеры

void main()
{
    posCamSpace = viewMatrix * modelMatrix * vec4(vertexPosition, 1.0); //преобразование координат вершины в систему координат камеры
    normalCamSpace = normalize(normalToCameraMatrix * vertexNormal); //преобразование нормали в систему координат камеры

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1.0);
}
