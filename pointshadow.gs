#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;	// Emit a triangle (3 vertices) for each of 6 faces

uniform mat4 lightSpaceMatrices[6];

out vec4 FragPos;

void main() {
	for (int face = 0; face < 6; ++face) {
		gl_Layer = face;	// built in variable that specifies cubemap face for rendering

		for (int i = 0; i < 3; i++) {
			FragPos = gl_in[i].gl_Position;
			gl_Position = lightSpaceMatrices[face] * FragPos;
			EmitVertex();
		}

		EndPrimitive();
	}
}