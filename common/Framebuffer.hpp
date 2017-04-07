#pragma once

#include <Texture.hpp>

#include <GL/glew.h>

#include <map>
#include <memory>

/**
Класс для управления фреймбуфером FrameBufferObject
*/
class Framebuffer
{
public:
    Framebuffer(unsigned int width, unsigned int height) :
        _width(width),
        _height(height)
    {
        glGenFramebuffers(1, &_fbo);
    }

    ~Framebuffer()
    {
        glDeleteFramebuffers(1, &_fbo);
    }

    void bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    }

    void unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    /**
    Возвращает ширину фреймбуфера
    */
    unsigned int width() const { return _width; }

    /**
    Возвращает высоту фреймбуфера
    */
    unsigned int height() const { return _height; }

    /**
    Создает текстуру заданного формата, добавляет к фреймбуферу в заданный аттачмент и возвращает текстуру
    */
    TexturePtr addBuffer(GLint internalFormat, GLenum attachment);

    /**
    Устанавливает буферы, куда осуществлять рендеринг
    */
    void initDrawBuffers();

    /**
    Изменяет размер текстур
    Это нужно при изменении размеров окна для тех фреймбуферов, которые должны совпадать по размерам с окном.
    */
    void resize(unsigned int width, unsigned int height);

    /**
    Проверят, настроен ли фреймбуфер корректно
    */
    bool valid() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        bool result = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return result;
    }

protected:
    Framebuffer(const Framebuffer&) = delete;
    void operator=(const Framebuffer&) = delete;

    GLuint _fbo;

    unsigned int _width;
    unsigned int _height;

    std::map<TexturePtr, GLenum> _textureToAttachment;
    std::map<TexturePtr, GLint> _textureToInternalFormat;

    std::vector<GLenum> _drawAttachments;
};

typedef std::shared_ptr<Framebuffer> FramebufferPtr;
