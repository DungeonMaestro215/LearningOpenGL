#pragma once

#include <glm/glm.hpp>

#include "light.h"

const glm::vec3 DEFAULT_DIR = glm::vec3(0.0f, -1.0f, 0.0f);

class DirLight : public virtual Light {
	public:
		glm::vec3 direction;

		DirLight(glm::vec3 ambient = glm::vec3(0.0f), glm::vec3 diffuse = glm::vec3(0.0f), glm::vec3 specular = glm::vec3(0.0f), 
			glm::vec3 direction = DEFAULT_DIR)
			: Light(ambient, diffuse, specular, "dirLight") {

			this->direction = direction;
		}

		void SendToShader(Shader &shader);
		void SendToShader(Shader &shader, int index);
};
