#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

/**
Параметры точечного источника света
*/
struct LightInfo
{
    ///Координаты источника света в мировой системе координат (при копировании в юниформ-переменную конвертировать в систему виртуальной камеры)
    glm::vec3 position;

    ///Цвет окружающего света
    glm::vec3 ambient;

    ///Диффузный цвет
    glm::vec3 diffuse;

    //Бликовый цвет
    glm::vec3 specular;

    //Константный коэффициент затухания
    float attenuation0;

    //Линейный коэффициент затухания
    float attenuation1;

    //Квадратичный коэффициент затухания
    float attenuation2;

    LightInfo() : attenuation0(1.0f), attenuation1(0.0f), attenuation2(0.0f) {}
};