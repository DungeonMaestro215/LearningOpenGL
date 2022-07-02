#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <glad/glad.h>

class EBO {
	public:
		GLuint ID;
		EBO() { ID = -1; }		// Maybe not the best default constructor...
		EBO(const void *indices, GLsizeiptr size);

		void Bind();
		void Unbind();
		void Delete();
};

#endif
