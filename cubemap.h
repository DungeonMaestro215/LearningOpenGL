#pragma once

#include "texture.h"

class Cubemap : public Texture {
	public:
		Cubemap(std::vector<std::string> faces, GLenum slot,
			GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT, GLint wrap_r = GL_REPEAT,
			GLenum pixelType = GL_UNSIGNED_BYTE,
			GLint minFilter = GL_NEAREST, GLint maxFilter = GL_NEAREST);

		Cubemap(int width, int height, GLenum format, GLenum internalFormat, GLenum slot,
			GLint wrap_s = GL_REPEAT, GLint wrap_t = GL_REPEAT, GLint wrap_r = GL_REPEAT,
			GLenum pixelType = GL_UNSIGNED_BYTE,
			GLint minFilter = GL_NEAREST, GLint maxFilter = GL_NEAREST);
};
