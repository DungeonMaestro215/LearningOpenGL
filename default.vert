#version 330 core

layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aColor;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

//out vec3 color;
out vec2 texCoord;
out vec3 normal;
out vec3 fragPosition;
//out vec3 lightPositionView;		// Light's position in view space
//out vec3 lightDirectionView;		// Light's direction in view space

//uniform vec3 lightPosition;
//uniform vec3 lightDirection;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	//gl_Position = transform * vec4(aPos.x + aPos.x*scale + xOffset, aPos.y + aPos.y*scale, aPos.z + aPos.z*scale, 1.0f);
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	//color = aColor;
	texCoord = aTex;
	
	// model space lighting 
	//fragPosition = vec3(model * vec4(aPos, 1.0f));
	//normal = mat3(transpose(inverse(model))) * aNormal;		// probably shouldn't calculate inverse here

	// view space lighting 
	fragPosition = vec3(view * model * vec4(aPos, 1.0f));
	normal = mat3(transpose(inverse(view * model))) * aNormal;		// probably shouldn't calculate inverse here
}
