#version 330 core

struct Material {
	//sampler2D diffuse;
	//sampler2D specular;
	//sampler2D emission;

	sampler2D texture_diffuse0;
	sampler2D texture_specular0;
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

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse; 
	vec3 specular;
};

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
#define NR_POINT_LIGHTS 4

struct SpotLight {
	vec3 position;
	vec3 direction;

	float constant;
	float linear;
	float quadratic;

	float cutOff;
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};


in vec3 color;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPosition;

out vec4 FragColor;

uniform Material material;
//uniform Light light;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform float gamma;

uniform mat4 view;

uniform vec3 viewPosition;

//uniform sampler2D tex0;
//uniform sampler2D tex1;
//uniform float mixAmt;


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);  
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPosition, vec3 viewDir);  
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPosition, vec3 viewDir);  


// *** This is unoptimized, as there are many repeated calculations and repeated retrieval of textures ***
void main() {
	//FragColor = mix(texture(tex0, texCoord), texture(tex1, texCoord), mixAmt);
	//FragColor = texture(tex0, texCoord * time) * vec4(color, 1.0f);
	//FragColor = vec4(color, 1.0f);
	

	// Emission lighting
	//float selector = 0.0f;
	//float border = 0.1f;
	//if (texCoord.x < border || texCoord.x > 1.0f - border || texCoord.y < border || texCoord.y > 1.0f - border)
		//selector = 0.0f;
	//vec3 emission = selector * vec3(texture(material.emission, texCoord));



	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(-fragPosition);	// viewer is at (0,0,0) in viewspace

	// Directional lighting
	vec3 result = CalcDirLight(dirLight, norm, viewDir);

	// Point lighting
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
		result += CalcPointLight(pointLights[i], norm, fragPosition, viewDir);

	// Spot lighting
	result += CalcSpotLight(spotLight, norm, fragPosition, viewDir);
	//vec3 result = CalcSpotLight(spotLight, norm, fragPosition, viewDir);

	FragColor = vec4(result, 1.0f);
	FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
	// Phong Lighting Model
	vec3 lightDir = normalize(vec3(view * vec4(-light.direction, 0.0f)));	// direction should point from object to light source

	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0f);

	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

	// Combine
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse0, texCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse0, texCoord));
	vec3 specular = light.specular * spec * vec3(texture(material.texture_specular0, texCoord));
	return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPosition, vec3 viewDir) {
	// Phong Lighting Model
	vec3 lightDir = normalize(vec3(view * vec4(light.position, 1.0f)) - fragPosition);	// fragPosition also in view space

	// Diffuse
	//float diff = dot(normal, lightDir);
	float diff = max(dot(normal, lightDir), 0.0f);

	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

	// Attenuation
	float distance = length(vec3(view * vec4(light.position, 1.0)) - fragPosition);		// make sure in view space
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	// Combine
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse0, texCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse0, texCoord));
	vec3 specular = light.specular * spec * vec3(texture(material.texture_specular0, texCoord));
	ambient *= attenuation;		
	diffuse *= attenuation;
	specular *= attenuation;
	return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPosition, vec3 viewDir) {
	// Phong Lighting Model
	vec3 lightDir = normalize(vec3(view * vec4(light.position, 1.0f)) - fragPosition);	// fragPosition also in view space

	// Diffuse
	//float diff = dot(normal, lightDir);
	float diff = max(dot(normal, lightDir), 0.0f);

	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

	// Attenuation
	float distance = length(vec3(view * vec4(light.position, 1.0)) - fragPosition);		// make sure in view space
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	// Calculate angle for spotlight
	float theta = dot(lightDir, normalize(vec3(view * vec4(-light.direction, 0.0f))));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0f, 1.0f);

	// Combine
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse0, texCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse0, texCoord));
	vec3 specular = light.specular * spec * vec3(texture(material.texture_specular0, texCoord));
	//ambient *= attenuation * intensity;		
	ambient *= attenuation;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	return ambient + diffuse + specular;
}
