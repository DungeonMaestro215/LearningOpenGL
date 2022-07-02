#pragma once

#include <glm/glm.hpp>

#include "pointlight.h"
#include "dirlight.h"

const float CUTOFF = glm::cos(glm::radians(12.5f));
const float OUTERCUTOFF = glm::cos(glm::radians(17.5f));

class SpotLight : public PointLight, public DirLight {
	public:
		float cutOff;
		float outerCutOff;

		SpotLight(glm::vec3 ambient = glm::vec3(0.0f), glm::vec3 diffuse = glm::vec3(0.0f), glm::vec3 specular = glm::vec3(0.0f), 
			glm::vec3 position = glm::vec3(0.0f), glm::vec3 direction = DEFAULT_DIR, 
			Model model = Model(), 
			float constant = CONSTANT, float linear = LINEAR, float quadratic = QUADRATIC, 
			float cutOff = CUTOFF, float outerCutOff = OUTERCUTOFF)
			:Light(ambient, diffuse, specular, "spotLight"), 
			PointLight(ambient, diffuse, specular, position, model, constant, linear, quadratic), 
			DirLight(ambient, diffuse, specular, direction) {

			this->cutOff = cutOff;
			this->outerCutOff = outerCutOff;
		}

		void SendToShader(Shader &shader);
		void SendToShader(Shader &shader, int index);
};
