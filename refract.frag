#version 330 core
out vec4 FragColor;

in vec3 worldNormal;
in vec3 worldPosition;

uniform vec3 viewPosition;
uniform samplerCube skybox;
uniform float refractRatio;

void main()
{             
    // Refraction
    //float ratio = 1.00f / 1.52f;
    vec3 I = normalize(worldPosition - viewPosition);
    vec3 R = refract(I, normalize(worldNormal), refractRatio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
