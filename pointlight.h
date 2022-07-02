#pragma once

#include <glm/glm.hpp>

#include "light.h"
#include "model.h"
#include "object.h"

//const float CONSTANT = 1.0f;
//const float LINEAR = 0.09f;
//const float QUADRATIC = 0.032f;

// Values found through some experimentation
const float CONSTANT = 0.2f;
const float LINEAR = 0.2f;
const float QUADRATIC = 0.2f;

class PointLight : public virtual Light, public Object {
	public:
		float constant;
		float linear;
		float quadratic;

		PointLight(glm::vec3 ambient = glm::vec3(0.0f), glm::vec3 diffuse = glm::vec3(0.0f), glm::vec3 specular = glm::vec3(0.0f), 
			glm::vec3 position = glm::vec3(0.0f), Model model = Model(), 
			float constant = CONSTANT, float linear = LINEAR, float quadratic = QUADRATIC)
			: Light(ambient, diffuse, specular, "pointLight"), Object(model, position) {

			this->constant = constant;
			this->linear = linear;
			this->quadratic = quadratic;
		}

		void SendToShader(Shader &shader);
		void SendToShader(Shader &shader, int index);
};
