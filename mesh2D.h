#pragma once

#include "mesh.h"

struct Vertex2D {
	glm::vec2 Position;
	glm::vec2 TexCoords;
};

class Mesh2D : public Mesh {
	public:
		std::vector<Vertex2D> vertices;

		Mesh2D(const std::vector<Vertex2D> &vertices, const std::vector<GLuint> &indices, const std::vector<Texture> &textures);
		//void Draw(Shader &shader);
};
