#pragma once

#include <glm/glm.hpp>

#include "shader.h"

class Light {
	public:
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		std::string type;

		Light(glm::vec3 ambient = glm::vec3(0.0f), glm::vec3 diffuse = glm::vec3(0.0f), glm::vec3 specular = glm::vec3(0.0f), std::string type = "light") {
			this->ambient = ambient;
			this->diffuse = diffuse;
			this->specular = specular;
			this->type = type;
		}

		// Pure virtual function for this abstract class
		void SendToShader(Shader &shader);
		void SendToShader(Shader &shader, int index);
};
