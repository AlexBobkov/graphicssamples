#include <iostream>
#include <vector>
#include <fstream>

#include <glimg/glimg.h>

#include "Texture.h"

GLuint Texture::loadTexture(const std::string& filename)
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
	catch (glimg::loaders::stb::StbLoaderException&)
	{
		std::cerr << "Failed to load texture " << filename << std::endl;;
		exit(1);
	}
	catch (glimg::loaders::dds::DdsLoaderException&)
	{
		std::cerr << "Failed to load texture " << filename << std::endl;;
		exit(1);
	}

	return texId;
}