#include "light.h"

// Make sure to activate shader before calling this
void Light::SendToShader(Shader &shader) {
	shader.SetVec3(type + ".ambient", ambient);
	shader.SetVec3(type + ".diffuse", diffuse);
	shader.SetVec3(type + ".specular", specular);
}

void Light::SendToShader(Shader &shader, int index) {
	shader.SetVec3(type + "[" + std::to_string(index) + "].ambient", ambient);
	shader.SetVec3(type + "[" + std::to_string(index) + "].diffuse", diffuse);
	shader.SetVec3(type + "[" + std::to_string(index) + "].specular", specular);
}
