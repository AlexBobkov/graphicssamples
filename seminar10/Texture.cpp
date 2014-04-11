#define _USE_MATH_DEFINES
#include <math.h>

#include <glimg/glimg.h>

#include "Texture.h"

bool gamma = false;

void getColorFromLinearPalette(float value, float& r, float& g, float& b);

GLuint Texture::loadTexture(std::string filename, bool withAlpha)
{
	GLuint texId;

	try
	{
		boost::shared_ptr<glimg::ImageSet> pImageSet;

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

		GLint internalFormat = gamma ? GL_SRGB8 : (withAlpha ? GL_RGBA8 : GL_RGB8);
		GLint format = withAlpha ? GL_RGBA : GL_RGB;

		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, dims.width, dims.height, 0, format, GL_UNSIGNED_BYTE, pImage.GetImageData());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	catch(glimg::loaders::stb::StbLoaderException&)
	{
		std::cerr << "Failed to load texture " << filename << std::endl;;
		exit(1);
	}
	catch(glimg::loaders::dds::DdsLoaderException&)
	{
		std::cerr << "Failed to load texture " << filename << std::endl;;
		exit(1);
	}

	return texId;
}

GLuint Texture::loadTextureWithMipmaps(std::string filename)
{
	GLuint texId;

	try
	{
		boost::shared_ptr<glimg::ImageSet> pImageSet;
		pImageSet.reset(glimg::loaders::dds::LoadFromFile(filename));

		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);

		GLint internalFormat = gamma ? GL_SRGB8 : GL_RGB8;

		for(int mipmapLevel = 0; mipmapLevel < pImageSet->GetMipmapCount(); mipmapLevel++)
		{
			glimg::SingleImage pImage = pImageSet->GetImage(mipmapLevel, 0, 0);
			glimg::Dimensions dims = pImage.GetDimensions();

			glTexImage2D(GL_TEXTURE_2D, mipmapLevel, internalFormat, dims.width, dims.height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, pImage.GetImageData());
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, pImageSet->GetMipmapCount() - 1);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	catch(glimg::loaders::dds::DdsLoaderException&)
	{
		std::cerr << "Failed to load texture " << filename << std::endl;;
		exit(1);
	}

	return texId;
}

GLuint Texture::makeCustomTexture()
{
	unsigned int width = 128;
	unsigned int height = 128;

	std::vector<unsigned char> data;

	for (unsigned int row = 0; row < height; row++)
	{
		for (unsigned int column = 0; column < width; column++)
		{
			float r, g, b;
			//getColorFromLinearPalette((float)column / width, r, g, b);
			r = 1.0;
			g = 0.0;
			b = 0.0;

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
	try
	{
		boost::shared_ptr<glimg::ImageSet> pImageSet;
		pImageSet.reset(glimg::loaders::stb::LoadFromFile(filename));

		glimg::SingleImage pImage = pImageSet->GetImage(0, 0, 0);
		glimg::Dimensions dims = pImage.GetDimensions();

		GLint internalFormat = gamma ? GL_SRGB8 : GL_RGB8;

		glTexImage2D(target, 0, internalFormat, dims.width, dims.height, 0, GL_RGB, GL_UNSIGNED_BYTE, pImage.GetImageData());
	}
	catch(glimg::loaders::stb::StbLoaderException&)
	{
		std::cerr << "Failed to load texture " << filename << std::endl;;
		exit(1);
	}
}

GLuint Texture::loadCubeTexture(std::string basefilename)
{
	GLuint texId;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texId);

	loadCubeTextureFace(basefilename + "/negx.jpg", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	loadCubeTextureFace(basefilename + "/posx.jpg", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	loadCubeTextureFace(basefilename + "/posy.jpg", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	loadCubeTextureFace(basefilename + "/negy.jpg", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
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

	GLuint texId;
	GLuint texBufferId;
	glGenBuffers(1, &texBufferId);
	glBindBuffer(GL_TEXTURE_BUFFER, texBufferId);	
	glBufferData(GL_TEXTURE_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_BUFFER, texId);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1 );                         // set 1-byte alignment

	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F_ARB, texBufferId);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	glBindTexture(GL_TEXTURE_BUFFER, 0);

	return texId;
}