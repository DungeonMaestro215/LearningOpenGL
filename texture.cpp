#include "texture.h"

//Texture::Texture(const char *path, std::string type, GLenum texType, GLenum slot, GLenum pixelType) {
//	this->texType = texType;
//	this->type = type;
//	this->path = path;
//
//	glGenTextures(1, &ID);
//	glActiveTexture(slot);
//	glBindTexture(texType, ID);
//
//	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
//	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);
//
//	// Extra lines for use GL_CLAMP_TO_BORDER
//	// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
//	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);
//
//	int imgWidth, imgHeight, nChannels;
//
//	stbi_set_flip_vertically_on_load(true);		// OpenGL interprets images differently than stb
//
//	unsigned char *data = stbi_load(path, &imgWidth, &imgHeight, &nChannels, 0);
//
//	// If successful, load in the image
//	if (data) {
//		GLenum format = GL_RGB; 
//		GLenum internalFormat = GL_SRGB; 
//		if (nChannels == 1) {
//			format = GL_RED;
//			internalFormat = GL_RED;
//		}
//		//else if (nChannels == 3)
//		//	format = GL_RGB;
//		else if (nChannels == 4) {
//			format = GL_RGBA;
//			internalFormat = GL_SRGB_ALPHA;
//		}
//
//		glTexImage2D(texType, 0, internalFormat, imgWidth, imgHeight, 0, format, pixelType, data);
//		glGenerateMipmap(texType);
//	} else {
//		std::cout << "Failed to load texture" << std::endl;
//	}
//
//	// Free at the end
//	stbi_image_free(data);
//
//	// Unbind to prevent accidental changes
//	glBindTexture(GL_TEXTURE_2D, 0);
//}

//void Texture::texUnit(Shader &shader, const char *uniform, GLuint unit) {
//	GLuint tex0UniID = glGetUniformLocation(shader.ID, uniform);
//	shader.Activate();
//	glUniform1i(tex0UniID, unit);
//}

void Texture::Bind() {
	glBindTexture(texType, ID);
}

void Texture::Unbind() {
	glBindTexture(texType, 0);
}

void Texture::Delete() {
	glDeleteTextures(1, &ID);
}