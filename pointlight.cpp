#include "pointlight.h"

// Make sure to activate shader before calling this
void PointLight::SendToShader(Shader &shader) {
	Light::SendToShader(shader);
	shader.SetFloat(type + ".constant", constant);
	shader.SetFloat(type + ".linear", linear);
	shader.SetFloat(type + ".quadratic", quadratic);
	shader.SetVec3(type + ".position", position);
}

void PointLight::SendToShader(Shader &shader, int index) {
	Light::SendToShader(shader, index);
	shader.SetFloat(type + "[" + std::to_string(index) + "].constant", constant);
	shader.SetFloat(type + "[" + std::to_string(index) + "].linear", linear);
	shader.SetFloat(type + "[" + std::to_string(index) + "].quadratic", quadratic);
	shader.SetVec3(type + "[" + std::to_string(index) + "].position", position);
}