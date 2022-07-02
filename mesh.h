#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "texture2D.h"
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"

//struct Texture {
//	unsigned int ID;
//	std::string type; 
//};


class Mesh {
	public:
		//std::vector<Vertex> vertices;
		std::vector<GLuint> indices;
		std::vector<Texture> textures;

		Mesh(const std::vector<GLuint> &indices = {}, const std::vector<Texture> &textures = {});
		//virtual void Draw(Shader &shader) = 0;
		void Draw(Shader &shader);
		//void Delete();

	//private:
		VAO myVAO;
		VBO myVBO;
		EBO myEBO;
};
