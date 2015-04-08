#pragma once

#include <string>
#include <vector>
#include <map>

#include <GL/glew.h>

/**
Создает и инициализирует Frame Buffer Object
*/
class Framebuffer
{
public:
    Framebuffer():
        _fboId(0),
        _width(1024),
        _height(1024)
    {        
    }

    ~Framebuffer()
    {
        if (_fboId != 0)
        {
            glDeleteFramebuffers(1, &_fboId);
        }
    }

    void create()
    {
        glGenFramebuffers(1, &_fboId);
    }

    void bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _fboId);
    }

    void unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    unsigned int width() const { return _width; }
    unsigned int height() const { return _height; }

    /**
    Устанавливает начальный размер текстур
    */
    void setSize(unsigned int width, unsigned int height) { _width = width; _height = height; }

    /**
    Изменяет размер текстур    
    */
    void resize(unsigned int width, unsigned int height);

    /**
    Создает текстуру заданного формата, добавляет к фреймбуферу в заданный аттачмент и возвращает id текстуры
    */
    GLuint addBuffer(GLint internalFormat, GLenum attachment);

    /**
    Устанавливает буферы, куда осуществлять рендеринг 
    */
    void initDrawBuffers();

    /**
    Проверят, настроен ли фреймбуфер корректно
    */
    bool valid() const { return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE; }
        	
protected:
	GLuint _fboId;

    unsigned int _width;
    unsigned int _height;
    
    std::map<GLuint, GLint> _texIdToFormat;
    std::map<GLuint, GLenum> _texIdToAttachment;
};
