#include "dirlight.h"

// Make sure to activate shader before calling this
void DirLight::SendToShader(Shader &shader) {
	Light::SendToShader(shader);
	shader.SetVec3(type + ".direction", direction);
}
void DirLight::SendToShader(Shader &shader, int index) {
	Light::SendToShader(shader, index);
	shader.SetVec3(type + "[" + std::to_string(index) + "].direction", direction);
}

