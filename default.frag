#version 330 core

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

struct Light {
	vec3 direction;
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;
};

in vec3 color;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPosition;

out vec4 FragColor;

uniform Material material;
uniform Light light;
uniform mat4 view;

//uniform vec3 viewPosition;

//uniform sampler2D tex0;
//uniform sampler2D tex1;
//uniform float mixAmt;

void main() {
	//FragColor = mix(texture(tex0, texCoord), texture(tex1, texCoord), mixAmt);
	//FragColor = texture(tex0, texCoord * time) * vec4(color, 1.0f);
	//FragColor = vec4(color, 1.0f);
	

	// Phong Lighting Model
	// Ambient lighting
	vec3 ambient = vec3(texture(material.diffuse, texCoord)) * light.ambient;

	// Diffuse Lighting
	vec3 lightDir = normalize(vec3(view * vec4(light.position, 1.0f)) - fragPosition);	// fragPosition also in view space
	//vec3 lightDir = normalize(vec3(view * vec4(-light.direction, 0.0f)));	// direction should point from object to light source
	vec3 norm = normalize(normal);
	//float diff = dot(norm, lightDir);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = diff * vec3(texture(material.diffuse, texCoord)) * light.diffuse;

	// Specular Lighting
	vec3 viewDir = normalize(-fragPosition);	// viewer is a (0,0,0) in viewspace
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	vec3 specular = spec * vec3(texture(material.specular, texCoord)) * light.specular;

	// Emission lighting
	float selector = 0.0f;
	//float border = 0.1f;
	//if (texCoord.x < border || texCoord.x > 1.0f - border || texCoord.y < border || texCoord.y > 1.0f - border)
		//selector = 0.0f;
	vec3 emission = selector * vec3(texture(material.emission, texCoord));


	// Calculate angle for spotlight
	float theta = dot(lightDir, normalize(vec3(view * vec4(-light.direction, 0.0f))));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);
	diffuse *= intensity;
	specular *= intensity;


	// Calculate Attenuation for point source
	float distance = length(vec3(view * vec4(light.position, 1.0)) - fragPosition);		// make sure in view space
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	ambient *= attenuation;		
	diffuse *= attenuation;
	specular *= attenuation;


	vec3 final = ambient + diffuse + specular + emission;
	//FragColor = vec4(final, 1.0f);
	FragColor = vec4(diffuse, 1.0f);
}
