#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "shader.h"

const GLfloat defaultBorderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };

class Texture {
	public:
		GLuint ID;
		GLenum texType;
		std::string path = "";
		std::string type = "";
		Texture() { ID = -1; texType = GL_TEXTURE_2D; }
		//Texture(const char *path, std::string type, GLenum texType, GLenum slot, GLenum pixelType);

		//void texUnit(Shader &shader, const char *uniform, GLuint unit);
		void Bind();
		void Unbind();
		void Delete();
};

#endif
