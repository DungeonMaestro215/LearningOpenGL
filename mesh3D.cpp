#include "mesh3D.h"

Mesh3D::Mesh3D(const std::vector<Vertex3D> &vertices, const std::vector<GLuint> &indices, const std::vector<Texture> &textures) 
: Mesh(indices, textures) {
    this->vertices = vertices;
    //this->indices = indices;
    //this->textures = textures;

    // Setup Mesh
    //myVAO = *new VAO();
	//myVBO = *new VBO(&vertices[0], vertices.size() * sizeof(Vertex));
    //myEBO = *new EBO(&indices[0], indices.size() * sizeof(GLuint));
	VBO myVBO(&vertices[0], vertices.size() * sizeof(Vertex3D));
    EBO myEBO(&indices[0], indices.size() * sizeof(GLuint));


    myVAO.Bind();
    myVBO.Bind();
    myEBO.Bind();
    myVAO.LinkAttrib(myVBO, 0, 3, GL_FLOAT, sizeof(Vertex3D), (void *)0);
    myVAO.LinkAttrib(myVBO, 1, 3, GL_FLOAT, sizeof(Vertex3D), (void *)offsetof(Vertex3D, Normal));
    myVAO.LinkAttrib(myVBO, 2, 2, GL_FLOAT, sizeof(Vertex3D), (void *)offsetof(Vertex3D, TexCoords));
    myVAO.Unbind();
    myVBO.Unbind();
    myEBO.Unbind();
}

// Make sure to activate the shader before calling draw
//void Mesh3D::Draw(Shader &shader) {
//    unsigned int diffuseNum = 1;
//    unsigned int specularNum = 1;
//
//    //std::cout << vertices[0].Normal.x << " " << vertices[0].Normal.y << " " << vertices[0].Normal.z << "\n";
//
//    for (unsigned int i = 0; i < textures.size(); i++) {
//        glActiveTexture(GL_TEXTURE0 + i);   // Activate texture unit
//
//        std::string number = "0";
//        std::string type = textures[i].type;
//        if (type == "texture_diffuse")
//            number = std::to_string(diffuseNum++);
//        else if (type == "texture_specular")
//            number = std::to_string(specularNum++);
//
//        //textures[i].texUnit(shader, ("material." + type + number).c_str(), i);
//        shader.SetInt("material." + type + number, i);
//        shader.SetFloat("material.shininess", 64.0f);
//        textures[i].Bind();
//    }
//
//    // Draw mesh
//    myVAO.Bind();
//    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
//    myVAO.Unbind();
//
//    // Set back to default
//    glActiveTexture(GL_TEXTURE0);
//}
