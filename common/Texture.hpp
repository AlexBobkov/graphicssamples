#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>

#include <string>
#include <vector>
#include <memory>

/**
Класс для управления текстурным объектом
*/
class Texture
{
public:
    /**
    Создает текстурный объект
    \param target тип текстуры (GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BUFFER и другие)
    */
    Texture(GLenum target = GL_TEXTURE_2D) :
        _target(target)
    {
        glGenTextures(1, &_tex);
    }

    /**
    Использует уже созданный текстурный объект
    */
    Texture(GLuint tex, GLenum target) :
        _tex(tex),
        _target(target)
    {
    }

    ~Texture()
    {
        glDeleteTextures(1, &_tex);
    }

    /**
    Копирует данные из оперативной памяти в видеопамять, выделяя память под данные при необходимости
    \param target тип текстуры (GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z и другие)
    \param level уровень детализации (мипмап-уровень), 0 - базовая текстура
    \param internalFormat внутренний формат хранения текстуры в видеопамяти (GL_RGB8 и другие)
    \param width ширина текстуры в пикселях
    \param height высота текстуры в пикселях
    \param format содержит количество компонетов цвета пикселя во входном массиве данных (GL_RED, GL_RG, GL_RGB, GL_RGBA и другие)
    \param type тип данных компонент цвета пикселя во входном массиве данных (GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT, GL_FLOAT и другие)
    \param data указатель на начало массива данных в оперативной памяти
    */
    void setTexImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* data)
    {
        glBindTexture(_target, _tex);
        glTexImage2D(target, level, internalFormat, width, height, 0, format, type, data);
        glBindTexture(_target, 0);
    }

    /**
    Автоматически создает мипмап-уровни для текстуры
    */
    void generateMipmaps()
    {
        glBindTexture(_target, _tex);
        glGenerateMipmap(_target);
        glBindTexture(_target, 0);
    }

    /**
    Прикрепляет текстуру к фреймбуферу (разбирается на 7м семинаре)
    */
    void attachToFramebuffer(GLenum attachment)
    {
        glBindTexture(_target, _tex);
        glFramebufferTexture(GL_FRAMEBUFFER, attachment, _tex, 0);
        glBindTexture(_target, 0);
    }

    void bind() const
    {
        glBindTexture(_target, _tex);
    }

    void unbind() const
    {
        glBindTexture(_target, 0);
    }

protected:
    Texture(const Texture&) = delete;
    void operator=(const Texture&) = delete;

    GLuint _tex;
    GLenum _target;
};

typedef std::shared_ptr<Texture> TexturePtr;

//=========== Функции для создания текстур

enum class SRGB
{
    YES,
    NO
};

/**
Загружает текстуру из файла PNG, JPEG и других форматов
*/
TexturePtr loadTexture(const std::string& filename, SRGB srgb = SRGB::NO);

/**
Загружает текстуру из файла DDS
*/
TexturePtr loadTextureDDS(const std::string& filename);

/**
Загружает кубическую текстуру
*/
TexturePtr loadCubeTexture(const std::string& basefilename);

/**
Пример программной генерации текстуры
*/
TexturePtr makeProceduralTexture();