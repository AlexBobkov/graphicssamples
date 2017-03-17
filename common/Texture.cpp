#include <Texture.hpp>

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
        for (int j = 0; j * 2 < height; ++j)
        {
            unsigned int index1 = j * width * channels;
            unsigned int index2 = (height - 1 - j) * width * channels;
            for (int i = 0; i < width * channels; i++)
            {
                unsigned char temp = image[index1];
                image[index1] = image[index2];
                image[index2] = temp;
                ++index1;
                ++index2;
            }
        }
    }
}

TexturePtr loadTexture(const std::string& filename, SRGB srgb)
{
    int width, height, channels;
    unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
    if (!image)
    {
        std::cerr << "SOIL loading error: " << SOIL_last_result() << std::endl;
        return std::make_shared<Texture>();
    }

    invertY(image, width, height, channels);

    GLint internalFormat;
    if (srgb == SRGB::YES)
    {
        internalFormat = (channels == 4) ? GL_SRGB8 : GL_SRGB8_ALPHA8;
    }
    else
    {
        internalFormat = (channels == 4) ? GL_RGBA8 : GL_RGB8;
    }

    GLint format = (channels == 4) ? GL_RGBA : GL_RGB;

    TexturePtr texture = std::make_shared<Texture>(GL_TEXTURE_2D);
    texture->setTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, format, GL_UNSIGNED_BYTE, image);
    texture->generateMipmaps();

    SOIL_free_image_data(image);

    return texture;
}

TexturePtr loadTextureDDS(const std::string& filename)
{
    GLuint tex = SOIL_load_OGL_texture(filename.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_DDS_LOAD_DIRECT);
    if (tex == 0)
    {
        std::cerr << "SOIL loading error: " << SOIL_last_result() << std::endl;
        return std::make_shared<Texture>();
    }

    return std::make_shared<Texture>(tex, GL_TEXTURE_2D);
}

//==========================================================

namespace
{
    //Удобная функция для вычисления цвета из линейной палитры от синего до красного
    glm::vec3 getColorFromLinearPalette(float value)
    {
        if (value < 0.25f)
        {
            return glm::vec3(0.0f, value * 4.0f, 1.0f);
        }
        else if (value < 0.5f)
        {
            return glm::vec3(0.0f, 1.0f, (0.5f - value) * 4.0f);
        }
        else if (value < 0.75f)
        {
            return glm::vec3((value - 0.5f) * 4.0f, 1.0f, 0.0f);
        }
        else
        {
            return glm::vec3(1.0f, (1.0f - value) * 4.0f, 0.0f);
        }
    }
}

TexturePtr makeProceduralTexture()
{
    int width = 128;
    int height = 128;

    std::vector<unsigned char> data;

    for (int row = 0; row < height; row++)
    {
        for (int column = 0; column < width; column++)
        {
            glm::vec3 color = getColorFromLinearPalette((float)column / width);

            data.push_back(static_cast<unsigned char>(255 * color.r));
            data.push_back(static_cast<unsigned char>(255 * color.g));
            data.push_back(static_cast<unsigned char>(255 * color.b));
        }
    }

    TexturePtr texture = std::make_shared<Texture>(GL_TEXTURE_2D);
    texture->setTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE, data.data());

    return texture;
}

//==========================================================

namespace
{
    void loadCubeTextureFace(const TexturePtr& texture, GLenum target, const std::string& filename)
    {
        int width, height, channels;
        unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
        if (!image)
        {
            std::cerr << "SOIL loading error: " << SOIL_last_result() << std::endl;
            return;
        }

        texture->setTexImage2D(target, 0, GL_RGB8, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);

        SOIL_free_image_data(image);
    }
}

TexturePtr loadCubeTexture(const std::string& basefilename)
{
    TexturePtr texture = std::make_shared<Texture>(GL_TEXTURE_CUBE_MAP);

    loadCubeTextureFace(texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, basefilename + "/negx.jpg");
    loadCubeTextureFace(texture, GL_TEXTURE_CUBE_MAP_POSITIVE_X, basefilename + "/posx.jpg");
    loadCubeTextureFace(texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, basefilename + "/negy.jpg");
    loadCubeTextureFace(texture, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, basefilename + "/posy.jpg");
    loadCubeTextureFace(texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, basefilename + "/negz.jpg");
    loadCubeTextureFace(texture, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, basefilename + "/posz.jpg");

    return texture;
}