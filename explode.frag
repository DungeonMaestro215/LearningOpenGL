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


//in vec3 color;
in vec2 texCoord;
in vec3 vertNormal;
in vec3 fragPosition;
in vec4 fragPositionLightSpace;

//out vec4 FragColor;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform Material material;
//uniform Light light;
uniform DirLight dirLight;
uniform PointLight pointLight[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

uniform mat4 view;

uniform vec3 viewPosition;

//uniform sampler2D tex0;
//uniform sampler2D tex1;
//uniform float mixAmt;

uniform float gamma;
uniform sampler2D shadowMap;

uniform samplerCube shadowCubemap;
uniform float far_plane;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);  
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPosition, vec3 viewDir, int index);  
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPosition, vec3 viewDir);  
float ShadowCalc(vec4 fragPositionLightSpace, float bias);
float PointShadowCalc(vec3 fragPosition, vec3 lightPosition, float bias);


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



	vec3 norm = normalize(vertNormal);
	vec3 viewDir = normalize(-fragPosition);	// viewer is at (0,0,0) in viewspace

	// Directional lighting
	vec3 result = CalcDirLight(dirLight, norm, viewDir);

	// Point lighting
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
		result += CalcPointLight(pointLight[i], norm, fragPosition, viewDir, i);

	// Spot lighting
	result += CalcSpotLight(spotLight, norm, fragPosition, viewDir);
	//vec3 result = CalcSpotLight(spotLight, norm, fragPosition, viewDir);

	FragColor = vec4(result, 1.0f);
	FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));

    // Brightness threshold for bloom effect
    float brightness = dot(FragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
    if (brightness > 1.0f)
        BrightColor = vec4(FragColor.rgb, 1.0f);
    else
        BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	//FragColor = vec4(vec3(ShadowCalc(fragPositionLightSpace)), 1.0f);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
	// Phong Lighting Model
	vec3 lightDir = normalize(vec3(view * vec4(-light.direction, 0.0f)));	// direction should point from object to light source

	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0f);

	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);

	// Get Shadow
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	float shadow = ShadowCalc(fragPositionLightSpace, bias);
	//shadow = 0.0f;

	// Combine
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse0, texCoord));
	vec3 diffuse = (1.0f - shadow) *  light.diffuse * diff * vec3(texture(material.texture_diffuse0, texCoord));
	vec3 specular = (1.0f - shadow) *  light.specular * spec * vec3(texture(material.texture_specular0, texCoord));
	return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPosition, vec3 viewDir, int index) {
	// Phong Lighting Model
	vec3 lightDir = normalize(vec3(view * vec4(light.position, 1.0f)) - fragPosition);	// fragPosition also in view space

	// Diffuse
	//float diff = dot(normal, lightDir);
	float diff = max(dot(normal, lightDir), 0.0f);

	// Specular
	//vec3 reflectDir = reflect(-lightDir, normal);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	vec3 halfwayDir = normalize(lightDir + viewDir);	// hopefully the sign is correct. 
	float spec = pow(max(dot(normal, halfwayDir), 0.0f), material.shininess);

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

	// Only mess with the 4th point light right now
	if (index == 3) {
		float bias = 0.05;
		float shadow = PointShadowCalc(fragPosition, vec3(view * vec4(light.position, 1.0f)), bias);
		//shadow = 0.0f;

		diffuse *= (1.0f - shadow);
		specular *= (1.0f - shadow);
	}

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

float ShadowCalc(vec4 fragPositionLightSpace, float bias) {
	// Start with perspective divide
	vec3 projCoords = fragPositionLightSpace.xyz / fragPositionLightSpace.w;

	// Transform to NDC
	projCoords = projCoords * 0.5 + 0.5;
	float currentDepth = projCoords.z;

	// If object is past frustum
	if (currentDepth > 1.0)
		return 0.0;

	// Sample shadow map
	//float closestDepth = texture(shadowMap, projCoords.xy).r;

	// Percentage-closer filtering (PCF)
	// Averages the 9 shadow values around the fragment
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;

	return shadow;
}

float PointShadowCalc(vec3 fragPosition, vec3 lightPosition, float bias) {
	// Vector which points from fragment to light
	vec3 fragToLight = fragPosition - lightPosition;	// done in view space

	// Sample shadow map and translate to the range [0, far_plane]
	float closestDepth = texture(shadowCubemap, fragToLight).r;
	closestDepth *= far_plane;

	//FragColor = vec4(vec3(closestDepth / far_plane), 1.0f);
	//FragColor = vec4(fragToLight, 1.0f);
	//FragColor = vec4(vec3(length(fragToLight)), 1.0f);
	
	// Current depth value
	float currentDepth = length(fragToLight);

	// Calc shadow value
	float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

	//return shadow;
	return 0.0f;
}
