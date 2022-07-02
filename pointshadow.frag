#version 330 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main() {
	// Distance between fragment and light source
	float lightDistance = length(FragPos.xyz - lightPos);

	// Map to [0,1]
	lightDistance = lightDistance / far_plane;

	// Write depth
	gl_FragDepth = lightDistance;
	//gl_FragDepth = 0.0f;
}