#include "object.h"

Object::Object(Model model, glm::vec3 position, glm::mat4 translationMatrix, glm::mat4 scaleMatrix, glm::mat4 rotationMatrix) {
	this->model = model;
	this->position = position;

	this->translationMatrix = glm::translate(translationMatrix, position);
	this->scaleMatrix = scaleMatrix;
	this->rotationMatrix = rotationMatrix;

	UpdateModelMatrix();
}

Object::Object(const char *path, glm::vec3 positio, glm::mat4 translationMatrix, glm::mat4 scaleMatrix, glm::mat4 rotationMatrix) {
	this->model = Model(path);
	this->position = position;

	this->translationMatrix = glm::translate(translationMatrix, position);
	this->scaleMatrix = scaleMatrix;
	this->rotationMatrix = rotationMatrix;

	UpdateModelMatrix();
}

// Create a duplicate of this object
Object Object::Duplicate(bool inPlace) {
	if (inPlace)
		return Object(this->model, this->position, this->translationMatrix, this->scaleMatrix, this->rotationMatrix);
		
	return Object(this->model);
}

// Make sure to activate shader before rendering
void Object::Render(Shader &shader) {
	shader.SetMat4("model", modelMatrix);
	model.Draw(shader);
}

void Object::Translate(glm::vec3 dir) {
	position = position + dir;
	translationMatrix = glm::translate(translationMatrix, dir);
	UpdateModelMatrix();
}

// Scale to the chosen multiple. Cannot be stacked
void Object::ScaleTo(float scale) {
	ScaleTo(scale, scale, scale);
}

void Object::ScaleTo(float scaleX, float scaleY, float scaleZ) {
	scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scaleX, scaleY, scaleZ));
	UpdateModelMatrix();
}

// Scales by the chosen multiple. This can be stacked.
void Object::ScaleBy(float scale) {
	ScaleBy(scale, scale, scale);
}

void Object::ScaleBy(float scaleX, float scaleY, float scaleZ) {
	scaleMatrix = glm::scale(scaleMatrix, glm::vec3(scaleX, scaleY, scaleZ));
	UpdateModelMatrix();
}

void Object::Rotate(float angle, glm::vec3 axis) {
	rotationMatrix = glm::rotate(rotationMatrix, angle, axis);
	UpdateModelMatrix();
}

// Recalculates the model matrix
void Object::UpdateModelMatrix() {
	modelMatrix = translationMatrix * scaleMatrix * rotationMatrix;
	//modelMatrix = translationMatrix * scaleMatrix * glm::mat4(1.0f);
}

