#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

//in vec3 Normal[];

out vec3 normal;
out vec3 fragPosition;

const float MAGNITUDE = 0.4f;

uniform mat4 projection;
uniform float time;

vec3 GetNormal();
vec4 explode(vec4 position, vec3 normal);
void GenerateLine(int index);

void main() {
	GenerateLine(0);
	GenerateLine(1);
	GenerateLine(2);
}

void GenerateLine(int index) {
	normal = GetNormal();
	vec4 pos = explode(gl_in[index].gl_Position, normal);

	//gl_Position = projection * gl_in[index].gl_Position;
	gl_Position = projection * pos;
	EmitVertex();

	//gl_Position = projection * (gl_in[index].gl_Position + vec4(Normal[index], 0.0) * MAGNITUDE);
	//gl_Position = projection * (gl_in[index].gl_Position + vec4(normal, 0.0) * MAGNITUDE);
	gl_Position = projection * (pos + vec4(normal, 0.0) * MAGNITUDE);
	EmitVertex();

	EndPrimitive();
}

vec3 GetNormal() {
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return -normalize(cross(a, b));
}  

vec4 explode(vec4 position, vec3 normal) {
    float magnitude = 2.0f;
    vec3 direction = normal * ((sin(time) + 1.0f) / 2.0f) * magnitude; 
    fragPosition = position.xyz + direction;
    return position + vec4(direction, 0.0f);
} 