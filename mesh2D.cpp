#include "mesh2D.h"

Mesh2D::Mesh2D(const std::vector<Vertex2D> &vertices, const std::vector<GLuint> &indices, const std::vector<Texture> &textures) 
: Mesh(indices, textures) {
    this->vertices = vertices;
    //this->indices = indices;
    //this->textures = textures;

    // Setup Mesh
    //myVAO = *new VAO();
	//myVBO = *new VBO(&vertices[0], vertices.size() * sizeof(Vertex));
    //myEBO = *new EBO(&indices[0], indices.size() * sizeof(GLuint));
	VBO myVBO(&vertices[0], vertices.size() * sizeof(Vertex2D));
    EBO myEBO(&indices[0], indices.size() * sizeof(GLuint));


    myVAO.Bind();
    myVBO.Bind();
    myEBO.Bind();
    myVAO.LinkAttrib(myVBO, 0, 2, GL_FLOAT, sizeof(Vertex2D), (void *)0);
    myVAO.LinkAttrib(myVBO, 1, 2, GL_FLOAT, sizeof(Vertex2D), (void *)offsetof(Vertex2D, TexCoords));
    myVAO.Unbind();
    myVBO.Unbind();
    myEBO.Unbind();
}
