#define _USE_MATH_DEFINES
#include <math.h>

#include <iostream>
#include <vector>
#include <fstream>

#include "Framebuffer.h"

GLuint Framebuffer::addBuffer(GLint internalFormat, GLenum attachment)
{
    GLuint texId;

    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    if (attachment == GL_DEPTH_ATTACHMENT)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, attachment, texId, 0);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0); //GL_RGB и GL_UNSIGNED_BYTE игнорируются, т.к. мы не копируем данные из оперативной памяти
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texId, 0);
    }    

    _texIdToFormat[texId] = internalFormat;
    _texIdToAttachment[texId] = attachment;

    return texId;
}

void Framebuffer::initDrawBuffers()
{
    std::vector<GLenum> attachments;

    for (std::map<GLuint, GLenum>::iterator itr = _texIdToAttachment.begin();
        itr != _texIdToAttachment.end();
        ++itr)
    {
        if (itr->second != GL_DEPTH_ATTACHMENT)
        {
            attachments.push_back(itr->second);
        }
    }

    glDrawBuffers(attachments.size(), attachments.data());
}

void Framebuffer::resize(unsigned int width, unsigned int height)
{
    _width = width;
    _height = height;

    glBindFramebuffer(GL_FRAMEBUFFER, _fboId);

    for (std::map<GLuint, GLint>::iterator itr = _texIdToFormat.begin();
        itr != _texIdToFormat.end();
        ++itr)
    {
        glBindTexture(GL_TEXTURE_2D, itr->first);

        if (_texIdToAttachment[itr->first] == GL_DEPTH_ATTACHMENT)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, itr->second, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}