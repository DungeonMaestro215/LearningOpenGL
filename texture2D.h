#pragma once

#include "texture.h"

class Texture2D : public Texture {
	public:
		Texture2D(const char *path, std::string type, GLenum slot, 
			GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT, 
			GLenum pixelType = GL_UNSIGNED_BYTE, 
			GLint minFilter = GL_NEAREST_MIPMAP_LINEAR, GLint maxFilter = GL_NEAREST, 
			const GLfloat borderColor[] = defaultBorderColor);

		Texture2D(int width, int height, GLenum format, GLenum interalFormat, std::string type, GLenum slot, 
			GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT, 
			GLenum pixelType = GL_UNSIGNED_BYTE, 
			GLint minFilter = GL_NEAREST_MIPMAP_LINEAR, GLint maxFilter = GL_NEAREST, 
			const GLfloat borderColor[] = defaultBorderColor);
		
};