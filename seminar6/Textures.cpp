#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "Application.h"

void getColorFromLinearPalette(float value, float& r, float& g, float& b);

GLuint Application::loadTexture(std::string filename) const
{
	GLuint texId;

	try
	{
		std::shared_ptr<glimg::ImageSet> pImageSet;

		if (filename.find(".dds") != std::string::npos)
		{
			pImageSet.reset(glimg::loaders::dds::LoadFromFile(filename));
		}
		else
		{
			pImageSet.reset(glimg::loaders::stb::LoadFromFile(filename));
		}

		glimg::SingleImage pImage = pImageSet->GetImage(0, 0, 0);
		glimg::Dimensions dims = pImage.GetDimensions();
		
		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, dims.width, dims.height, 0, GL_RGB, GL_UNSIGNED_BYTE, pImage.GetImageData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	catch(glimg::loaders::stb::StbLoaderException& e)
	{
		std::cerr << "Failed to load texture " << filename << std::endl;;
		exit(1);
	}
	catch(glimg::loaders::dds::DdsLoaderException& e)
	{
		std::cerr << "Failed to load texture " << filename << std::endl;;
		exit(1);
	}

	return texId;
}

GLuint Application::loadTextureWithMipmaps(std::string filename) const
{
	GLuint texId;

	try
	{
		std::shared_ptr<glimg::ImageSet> pImageSet;
		pImageSet.reset(glimg::loaders::dds::LoadFromFile(filename));

		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);

		for(int mipmapLevel = 0; mipmapLevel < pImageSet->GetMipmapCount(); mipmapLevel++)
		{
			glimg::SingleImage pImage = pImageSet->GetImage(mipmapLevel, 0, 0);
			glimg::Dimensions dims = pImage.GetDimensions();

			glTexImage2D(GL_TEXTURE_2D, mipmapLevel, GL_RGB8, dims.width, dims.height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, pImage.GetImageData());
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, pImageSet->GetMipmapCount() - 1);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	catch(glimg::loaders::dds::DdsLoaderException& e)
	{
		std::cerr << "Failed to load texture " << filename << std::endl;;
		exit(1);
	}

	return texId;
}

GLuint Application::makeCustomTexture() const
{
	int width = 128;
	int height = 128;

	std::vector<unsigned char> data;

	for (unsigned int row = 0; row < height; row++)
	{
		for (unsigned int column = 0; column < width; column++)
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