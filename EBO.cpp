#include "EBO.h"

EBO::EBO(const void *indices, GLsizeiptr size) {
	glGenBuffers(1, &ID);	// create the buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);	// bind this to the GL_ARRAY_BUFFER
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);	// send data to buffer
}

void EBO::Bind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);	// bind this to the GL_ARRAY_BUFFER
}

void EBO::Unbind() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::Delete() {
	glDeleteBuffers(1, &ID);
}
