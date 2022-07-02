#version 330 core

struct Material {
	//sampler2D diffuse;
	//sampler2D specular;
	//sampler2D emission;
	//float shininess;

	sampler2D texture_diffuse0;
	sampler2D texture_specular0;
};


in vec3 color;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPosition;

out vec4 FragColor;

uniform Material material;
//uniform Light light;

uniform mat4 view;

uniform vec3 viewPosition;

//uniform sampler2D tex0;
//uniform sampler2D tex1;
//uniform float mixAmt;


void main() {
	//FragColor = mix(texture(tex0, texCoord), texture(tex1, texCoord), mixAmt);
	//FragColor = texture(tex0, texCoord * time) * vec4(color, 1.0f);
	
	//FragColor = vec4(result, 1.0f);

	vec3 result = vec3(texture(material.texture_diffuse0, texCoord));
	FragColor = vec4(result, 1.0f);
}
