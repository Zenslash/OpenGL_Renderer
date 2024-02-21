#pragma once

#include <glad/glad.h>
#include <vector>
#include <string>
#include <GLFW/glfw3.h>
#include <iostream>

#include "stb_image.h"

static unsigned int loadCubemap(const std::vector<std::string>& cubeFaces)
{
	unsigned int cubemapID;
	glGenTextures(1, &cubemapID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapID);

	int texWidth, texHeight, nrChannels;
	unsigned char* texData;
	for (unsigned int i = 0; i < cubeFaces.size(); i++)
	{
		texData = stbi_load(cubeFaces[i].c_str(), &texWidth, &texHeight, &nrChannels, 0);
		if (texData)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
			stbi_image_free(texData);
		}
		else
		{
			std::cout << "Cubemap texture loading failed at path: " + cubeFaces[i] << std::endl;
			stbi_image_free(texData);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return cubemapID;
}