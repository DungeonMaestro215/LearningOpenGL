#version 330 core

layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;
layout (location = 3) in mat4 instanceMatrix;

//out vec3 color;
out vec2 texCoord;
out vec3 normal;
out vec3 fragPosition;
out vec4 fragPositionLightSpace;
//out vec3 lightPositionView;		// Light's position in view space
//out vec3 lightDirectionView;		// Light's direction in view space
out vec3 worldNormal;
out vec3 worldPosition;

//uniform vec3 lightPosition;
//uniform vec3 lightDirection;

//uniform mat4 models[1000];
//uniform vec3 offsets[1000];
//uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

void main() {
	//gl_Position = transform * vec4(aPos.x + aPos.x*scale + xOffset, aPos.y + aPos.y*scale, aPos.z + aPos.z*scale, 1.0f);
	//model = translate(model, positions[gl_InstanceID]);
	mat4 model = instanceMatrix;
	//vec3 Offset = gl_InstanceID * vec3(1.0f, 0.0f, 0.0f);
	//gl_Position = projection * view * (model * vec4(aPos, 1.0f) + vec4(sin(gl_InstanceID * time), cos(gl_InstanceID * time), sin(gl_InstanceID * time * time), 0.0f));
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	//gl_Position = projection * (view * model * vec4(aPos + vec3(sin(time), cos(time), 0.0f), 1.0f));
	//color = aColor;
	texCoord = aTex;
	
	// model space lighting 
	//fragPosition = vec3(model * vec4(aPos, 1.0f));
	//normal = mat3(transpose(inverse(model))) * aNormal;		// probably shouldn't calculate inverse here

	// view space lighting 
	fragPosition = vec3(view * model * vec4(aPos, 1.0f));
	normal = mat3(transpose(inverse(view * model))) * aNormal;		// probably shouldn't calculate inverse here

	// World space for reflections
	worldPosition = vec3(model * vec4(aPos, 1.0f));
	worldNormal = mat3(transpose(inverse(model))) * aNormal;		// probably shouldn't calculate inverse here

	// Position used for shadows
	fragPositionLightSpace = lightSpaceMatrix * vec4(vec3(model * vec4(aPos, 1.0f)), 1.0f);
}
