#version 330 core

layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

//out vec3 color;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPosition;
out vec4 FragPositionLightSpace;
//out vec3 lightPositionView;		// Light's position in view space
//out vec3 lightDirectionView;		// Light's direction in view space
out vec3 WorldNormal;
out vec3 WorldPosition;

//uniform vec3 lightPosition;
//uniform vec3 lightDirection;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main() {
	//gl_Position = transform * vec4(aPos.x + aPos.x*scale + xOffset, aPos.y + aPos.y*scale, aPos.z + aPos.z*scale, 1.0f);
	gl_Position = view * model * vec4(aPos, 1.0f);
	//color = aColor;
	TexCoord = aTex;
	
	// model space lighting 
	//fragPosition = vec3(model * vec4(aPos, 1.0f));
	//normal = mat3(transpose(inverse(model))) * aNormal;		// probably shouldn't calculate inverse here

	// view space lighting 
	FragPosition = vec3(view * model * vec4(aPos, 1.0f));
	mat3 normalMatrix = mat3(transpose(inverse(view * model)));		// probably shouldn't calculate inverse here
	Normal = normalize(normalMatrix * aNormal);		

	// World space for reflections
	WorldPosition = vec3(model * vec4(aPos, 1.0f));
	WorldNormal = mat3(transpose(inverse(model))) * aNormal;		// probably shouldn't calculate inverse here

	// Position used for shadows
	FragPositionLightSpace = lightSpaceMatrix * vec4(vec3(model * vec4(aPos, 1.0f)), 1.0f);
}
