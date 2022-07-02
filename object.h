#pragma once

#include <glm/glm.hpp>

#include "model.h"

class Object {
	public:
		Model model;

		glm::vec3 position;
		//float scale;

		glm::mat4 translationMatrix;
		glm::mat4 rotationMatrix;
		glm::mat4 scaleMatrix;
		glm::mat4 modelMatrix;

		Object(Model model = Model(), glm::vec3 position = glm::vec3(0.0f), glm::mat4 translationMatrix = glm::mat4(1.0f), glm::mat4 scaleMatrix = glm::mat4(1.0f), glm::mat4 rotationMatrix = glm::mat4(1.0f));
		Object(const char *path, glm::vec3 position = glm::vec3(0.0f), glm::mat4 translationMatrix = glm::mat4(1.0f), glm::mat4 scaleMatrix = glm::mat4(1.0f), glm::mat4 rotationMatrix = glm::mat4(1.0f));

		Object Duplicate(bool inPLace = true);
		void Render(Shader &shader);
		void Translate(glm::vec3 dir);
		void ScaleTo(float scale);
		void ScaleTo(float scaleX, float scaleY, float scaleZ);
		void ScaleBy(float scale);
		void ScaleBy(float scaleX, float scaleY, float scaleZ);
		void Rotate(float angle, glm::vec3 axis);
		void UpdateModelMatrix();
};
