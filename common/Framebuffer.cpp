#include <Framebuffer.hpp>

#include <iostream>
#include <vector>

TexturePtr Framebuffer::addBuffer(GLint internalFormat, GLenum attachment)
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    //Создаем текстуру
    TexturePtr texture = std::make_shared<Texture>();

    //Выделяем под нее память
    if (attachment == GL_DEPTH_ATTACHMENT)
    {
        texture->setTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, _width, _height, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    }
    else
    {
        texture->setTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, 0); //GL_RGB и GL_UNSIGNED_BYTE игнорируются, т.к. мы не копируем данные из оперативной памяти        
    }

    //Прикрепляем текстуру к фреймбуферу
    texture->attachToFramebuffer(attachment);

    _textureToAttachment[texture] = attachment;
    _textureToInternalFormat[texture] = internalFormat;

    if (attachment != GL_DEPTH_ATTACHMENT)
    {
        _drawAttachments.push_back(attachment);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return texture;
}

void Framebuffer::initDrawBuffers()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    if (_drawAttachments.size() > 0)
    {
        glDrawBuffers(_drawAttachments.size(), _drawAttachments.data());
    }
    else
    {
        GLenum buffers[] = { GL_NONE };
        glDrawBuffers(1, buffers);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::resize(unsigned int width, unsigned int height)
{
    _width = width;
    _height = height;

    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    //Пробегаем по всем текстурам и по новой выделяем память под нужный размер
    for (const auto& kv : _textureToInternalFormat)
    {
        if (_textureToAttachment[kv.first] == GL_DEPTH_ATTACHMENT)
        {
            kv.first->setTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, _width, _height, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        }
        else
        {
            kv.first->setTexImage2D(GL_TEXTURE_2D, 0, kv.second, _width, _height, GL_RGB, GL_UNSIGNED_BYTE, 0);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}