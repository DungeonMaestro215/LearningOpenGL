#pragma once

#include "mesh.h"

struct Vertex3D {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

class Mesh3D : public Mesh {
	public:
		std::vector<Vertex3D> vertices;

		Mesh3D(const std::vector<Vertex3D> &vertices, const std::vector<GLuint> &indices, const std::vector<Texture> &textures);
		//void Draw(Shader &shader);
};
