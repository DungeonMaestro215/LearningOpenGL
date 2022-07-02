#include "VAO.h"

VAO::VAO() {
	glGenVertexArrays(1, &ID);	// create the array
}

void VAO::LinkAttrib(VBO &VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void *offset) {
	VBO.Bind();
	// Tell OpenGL how the data should be interpreted (as it is just one big array)
	glEnableVertexAttribArray(layout);
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);	// stride 0 indicates that attributes are tightly packed
	VBO.Unbind();
}

void VAO::Bind() {
	glBindVertexArray(ID);	// bind this to the GL_ARRAY_BUFFER
}

void VAO::Unbind() {
	glBindVertexArray(0);
}

void VAO::Delete() {
	glDeleteVertexArrays(1, &ID);
}
