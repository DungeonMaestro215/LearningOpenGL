#include "spotlight.h"

// Make sure to activate shader before calling this
void SpotLight::SendToShader(Shader &shader) {
	PointLight::SendToShader(shader);
	DirLight::SendToShader(shader);

	shader.SetFloat(type + ".cutOff", cutOff);
	shader.SetFloat(type + ".outerCutOff", outerCutOff);
}

void SpotLight::SendToShader(Shader &shader, int index) {
	PointLight::SendToShader(shader, index);
	DirLight::SendToShader(shader, index);

	shader.SetFloat(type + "[" + std::to_string(index) + "].cutOff", cutOff);
	shader.SetFloat(type + "[" + std::to_string(index) + "].outerCutOff", outerCutOff);
}
