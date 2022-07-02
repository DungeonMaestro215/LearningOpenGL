#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

std::string get_file_contents(const char* filename);

class Shader {
	public:
		GLuint ID;
		Shader() { ID = -1; }
		Shader(const char *vertexFile, const char *fragmentFile, const char *geometryFile = nullptr);
		//Shader(const char *vertexFile, const char *fragmentFile, const char *geometryFile);

		void Activate();
		void SetBool(const std::string &name, bool value);
		void SetInt(const std::string &name, int value);
		void SetFloat(const std::string &name, float value);
		void SetVec2(const std::string &name, glm::vec2 value);
		void SetVec2(const std::string &name, float x, float y);
		void SetVec3(const std::string &name, glm::vec3 value);
		void SetVec3(const std::string &name, float x, float y, float z);
		void SetVec4(const std::string &name, glm::vec4 value);
		void SetVec4(const std::string &name, float x, float y, float z, float w);
		void SetMat2(const std::string &name, glm::mat2 value);
		void SetMat3(const std::string &name, glm::mat3 value);
		void SetMat4(const std::string &name, glm::mat4 value);
		void Delete();
		void checkErrors(GLuint shader, const char *type);
};

#endif
