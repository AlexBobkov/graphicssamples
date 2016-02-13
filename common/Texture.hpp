#pragma once

#include <Mesh.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h>

#include <string>
#include <vector>
#include <memory>

/**
Загружает текстуры с диска в оперативную и затем в видеопамять.
Возвращает идентификатор текстурного объекта.
*/
class Texture
{
public:
    static GLuint loadTexture(const std::string& filename, bool gamma = false, bool withAlpha = false);

    static GLuint loadTextureDDS(const std::string& filename);

    static GLuint loadCubeTexture(const std::string& basefilename);
};

/**
Абстракция буфера с данными в видеопамяти
*/
class Texture2
{
public:
    /**
    Создает текстурный объект
    \param target тип текстуры (GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BUFFER и другие)
    */
    Texture2(GLenum target = GL_TEXTURE_2D) :
        _target(target)
    {
        glGenTextures(1, &_tex);
    }

    /**
    Использует уже созданный текстурный объект
    */
    Texture2(GLuint tex, GLenum target) :
        _tex(tex),
        _target(target)
    {        
    }

    ~Texture2()
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
        glTexImage2D(_target, level, internalFormat, width, height, 0, format, type, data);
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

    void bind() const
    {
        glBindTexture(_target, _tex);
    }

    void unbind() const
    {
        glBindTexture(_target, 0);
    }

protected:
    Texture2(const Texture2&) = delete;
    void operator=(const Texture2&) = delete;

    GLuint _tex;
    GLenum _target;
};

typedef std::shared_ptr<Texture2> TexturePtr;

//=========== Функции для создания текстур

/**
Загружает текстуру из файла PNG, JPEG и других форматов
*/
TexturePtr loadTexture(const std::string& filename, bool gamma = false, bool withAlpha = false);

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