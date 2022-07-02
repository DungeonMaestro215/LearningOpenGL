#ifndef	MODEL_CLASS_H 
#define	MODEL_CLASS_H

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "shader.h"
#include "mesh2D.h"
#include "mesh3D.h"

class Model {
	public:
		std::vector<Texture> textures_loaded;	// optimization to make sure not to reload in textures

		Model() {}
		Model(const char *path) { loadModel(path); }
		Model(Mesh mesh) { meshes.push_back(mesh);  }
		Model(const std::vector<Vertex3D> &vertices, const std::vector<GLuint> &indices, const std::vector<Texture> &textures = {}) {
			meshes.push_back(Mesh3D(vertices, indices, textures));
		}
		Model(const std::vector<Vertex2D> &vertices, const std::vector<GLuint> &indices, const std::vector<Texture> &textures = {}) {
			meshes.push_back(Mesh2D(vertices, indices, textures));
		}
		//Model(GLfloat *vertices, GLuint *indices, Texture *textures);
		Model(std::vector<Mesh> &meshes) { this->meshes = meshes; }
		void Draw(Shader &shader);
		//void Delete();

	//private:
		std::vector<Mesh> meshes;
		std::string directory;

		void loadModel(std::string path);
		void processNode(aiNode *node, const aiScene *scene);
		Mesh processMesh(aiMesh *mesh, const aiScene *scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif
