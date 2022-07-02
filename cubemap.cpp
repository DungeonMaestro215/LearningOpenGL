#include <vector>

#include "cubemap.h"

Cubemap::Cubemap(std::vector<std::string> faces, GLenum slot, 
	GLint wrap_s, GLint wrap_t, GLint wrap_r, 
	GLenum pixelType, 
	GLint minFilter, GLint maxFilter
) {

	this->type = "texture_cubemap";
	//this->path = "";

	texType = GL_TEXTURE_CUBE_MAP;

	glGenTextures(1, &ID);
	glActiveTexture(slot);
	glBindTexture(texType, ID);

	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, maxFilter);

	glTexParameteri(texType, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, wrap_t);
	glTexParameteri(texType, GL_TEXTURE_WRAP_R, wrap_r);

	int imgWidth, imgHeight, nChannels;

	stbi_set_flip_vertically_on_load(false);		// OpenGL interprets images differently than stb

	for (unsigned int i = 0; i < faces.size(); i++) {
		//stbi_set_flip_vertically_on_load(true);		// OpenGL interprets images differently than stb
		unsigned char *data = stbi_load(faces[i].c_str(), &imgWidth, &imgHeight, &nChannels, 0);

		// If successful, load in the image
		if (data) {
			GLenum format = GL_RGB;
			GLenum internalFormat = GL_SRGB;
			if (nChannels == 1) {
				format = GL_RED;
				internalFormat = GL_RED;
			}
			//else if (nChannels == 3)
			//	format = GL_RGB;
			else if (nChannels == 4) {
				format = GL_RGBA;
				internalFormat = GL_SRGB_ALPHA;
			}

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, imgWidth, imgHeight, 0, format, pixelType, data);
		}
		else {
			std::cout << "Failed to load cubemap texture: " << faces[i] << std::endl;
		}

		// Free at the end
		stbi_image_free(data);
	}

	// Unbind to prevent accidental changes
	glBindTexture(texType, 0);
}

Cubemap::Cubemap(int width, int height, GLenum format, GLenum internalFormat, GLenum slot, 
	GLint wrap_s, GLint wrap_t, GLint wrap_r, 
	GLenum pixelType, 
	GLint minFilter, GLint maxFilter
) {

	this->type = "texture_cubemap";
	//this->path = "";

	texType = GL_TEXTURE_CUBE_MAP;

	glGenTextures(1, &ID);
	glActiveTexture(slot);
	glBindTexture(texType, ID);

	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, maxFilter);

	glTexParameteri(texType, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, wrap_t);
	glTexParameteri(texType, GL_TEXTURE_WRAP_R, wrap_r);


	for (unsigned int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, pixelType, NULL);
	}

	// Unbind to prevent accidental changes
	glBindTexture(texType, 0);
}