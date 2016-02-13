#include <Texture.hpp>

#include <glimg/glimg.h>

#include <SOIL2.h>

#include <vector>
#include <iostream>

namespace
{
    /**
    Библиотека SOIL читает текстуры перевернутыми
    */
    void invertY(unsigned char* image, int width, int height, int channels)
    {
        for (unsigned j = 0; j * 2 < height; ++j)
        {
            unsigned int index1 = j * width * channels;
            unsigned int index2 = (height - 1 - j) * width * channels;
            for (unsigned i = 0; i < width * channels; i++)
            {
                unsigned char temp = image[index1];
                image[index1] = image[index2];
                image[index2] = temp;
                ++index1;
                ++index2;
            }
        }
    }

    //Удобная функция для вычисления цвета из линейной палитры от синего до красного
    void getColorFromLinearPalette(float value, float& r, float& g, float& b)
    {
        if (value < 0.25f)
        {
            r = 0.0f;
            g = value * 4.0f;
            b = 1.0f;
        }
        else if (value < 0.5f)
        {
            r = 0.0f;
            g = 1.0f;
            b = (0.5f - value) * 4.0f;
        }
        else if (value < 0.75f)
        {
            r = (value - 0.5f) * 4.0f;
            g = 1.0f;
            b = 0.0f;
        }
        else
        {
            r = 1.0f;
            g = (1.0f - value) * 4.0f;
            b = 0.0f;
        }
    }
}

GLuint Texture::loadTexture(const std::string& filename, bool gamma, bool withAlpha)
{
    GLint internalFormat = gamma ? GL_SRGB8 : (withAlpha ? GL_RGBA8 : GL_RGB8);
    GLint format = withAlpha ? GL_RGBA : GL_RGB;

    GLuint texId;
    glGenTextures(1, &texId);

    int width, height, channels;
    unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, &channels, withAlpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
    if (!image)
    {
        std::cerr << "SOIL loading error: " << SOIL_last_result() << std::endl;
        return 0;
    }

    invertY(image, width, height, withAlpha ? 4 : 3);

    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    SOIL_free_image_data(image);

    return texId;
}

GLuint Texture::loadTextureDDS(const std::string& filename)
{
    GLuint texId = SOIL_load_OGL_texture(filename.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_DDS_LOAD_DIRECT);
    if (texId == 0)
    {
        std::cerr << "SOIL loading error: " << SOIL_last_result() << std::endl;
        return 0;
    }

    return texId;
}

GLuint Texture::makeProceduralTexture()
{
    int width = 128;
    int height = 128;

    std::vector<unsigned char> data;

    for (int row = 0; row < height; row++)
    {
        for (int column = 0; column < width; column++)
        {
            float r, g, b;
            getColorFromLinearPalette((float)column / width, r, g, b);

            data.push_back((unsigned char)(255 * r));
            data.push_back((unsigned char)(255 * g));
            data.push_back((unsigned char)(255 * b));
        }
    }

    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texId;
}

void loadCubeTextureFace(std::string filename, GLenum target)
{
    int width, height, channels;
    unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
    if (!image)
    {
        std::cerr << "SOIL loading error: " << SOIL_last_result() << std::endl;
        return;
    }

    invertY(image, width, height, 3);

    glTexImage2D(target, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    SOIL_free_image_data(image);
}

GLuint Texture::loadCubeTexture(const std::string& basefilename)
{
    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texId);

    loadCubeTextureFace(basefilename + "/negx.jpg", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
    loadCubeTextureFace(basefilename + "/posx.jpg", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
    loadCubeTextureFace(basefilename + "/negy.jpg", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
    loadCubeTextureFace(basefilename + "/posy.jpg", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
    loadCubeTextureFace(basefilename + "/negz.jpg", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
    loadCubeTextureFace(basefilename + "/posz.jpg", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return texId;
}

GLuint Texture::makeTextureBuffer(std::vector<glm::vec3>& positions)
{
    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); i++)
    {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
    }

    GLuint texBufferId;
    glGenBuffers(1, &texBufferId);
    glBindBuffer(GL_TEXTURE_BUFFER, texBufferId);
    glBufferData(GL_TEXTURE_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_BUFFER, texId);

    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F_ARB, texBufferId);

    glBindBuffer(GL_TEXTURE_BUFFER, 0);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

    return texId;
}