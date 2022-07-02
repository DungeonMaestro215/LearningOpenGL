#include "texture2D.h"

Texture2D::Texture2D(const char *path, std::string type, GLenum slot, 
	GLint wrap_s, GLint wrap_t, 
	GLenum pixelType, 
	GLint minFilter, GLint maxFilter, 
	const GLfloat borderColor[]
) {

	this->type = type;
	this->path = path;

	texType = GL_TEXTURE_2D;

	glGenTextures(1, &ID);
	glActiveTexture(slot);
	glBindTexture(texType, ID);

	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, maxFilter);

	glTexParameteri(texType, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, wrap_t);

	// Extra lines for use GL_CLAMP_TO_BORDER
	if (wrap_s == GL_CLAMP_TO_BORDER || wrap_t == GL_CLAMP_TO_BORDER)
		glTexParameterfv(texType, GL_TEXTURE_BORDER_COLOR, borderColor);

	int imgWidth, imgHeight, nChannels;

	stbi_set_flip_vertically_on_load(true);		// OpenGL interprets images differently than stb

	unsigned char *data = stbi_load(path, &imgWidth, &imgHeight, &nChannels, 0);

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

		glTexImage2D(texType, 0, internalFormat, imgWidth, imgHeight, 0, format, pixelType, data);
		glGenerateMipmap(texType);
	} else {
		std::cout << "Failed to load texture: " << path << std::endl;
	}

	// Free at the end
	stbi_image_free(data);

	// Unbind to prevent accidental changes
	glBindTexture(texType, 0);
}

Texture2D::Texture2D(int width, int height, GLenum format, GLenum internalFormat, std::string type, GLenum slot, 
	GLint wrap_s, GLint wrap_t, 
	GLenum pixelType, 
	GLint minFilter, GLint maxFilter, 
	const GLfloat borderColor[]
) {

	this->type = type;
	//this->path = path;

	texType = GL_TEXTURE_2D;

	glGenTextures(1, &ID);
	glActiveTexture(slot);
	glBindTexture(texType, ID);

	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, maxFilter);

	glTexParameteri(texType, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, wrap_t);

	// Extra lines for use GL_CLAMP_TO_BORDER
	if (wrap_s == GL_CLAMP_TO_BORDER || wrap_t == GL_CLAMP_TO_BORDER)
		glTexParameterfv(texType, GL_TEXTURE_BORDER_COLOR, borderColor);


	glTexImage2D(texType, 0, internalFormat, width, height, 0, format, pixelType, NULL);
	glGenerateMipmap(texType);

	// Unbind to prevent accidental changes
	glBindTexture(texType, 0);
}
