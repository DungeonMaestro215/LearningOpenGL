#include "VBO.h"

VBO::VBO(const void *data, GLsizeiptr size) {
	glGenBuffers(1, &ID);	// create the buffer
	glBindBuffer(GL_ARRAY_BUFFER, ID);	// bind this to the GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);	// send data to buffer
}

void VBO::Bind() {
	glBindBuffer(GL_ARRAY_BUFFER, ID);	// bind this to the GL_ARRAY_BUFFER
}

void VBO::Unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete() {
	glDeleteBuffers(1, &ID);
}

void VBO::Update(void *data, GLsizeiptr size) {
	glBindBuffer(GL_ARRAY_BUFFER, ID);	// bind this to the GL_ARRAY_BUFFER
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);	// send data to buffer
}
