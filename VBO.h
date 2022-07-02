#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <iostream>

class VBO {
	public:
		GLuint ID;
		VBO() { ID = -1; };		// Maybe not the best default constructor...
		VBO(const void *data, GLsizeiptr size);
		//VBO(glm::vec3 *vertices, GLsizeiptr size);
		//VBO(glm::mat4 *vertices, GLsizeiptr size);

		void Bind();
		void Unbind();
		void Delete();

		void Update(void *data, GLsizeiptr size);
};

#endif