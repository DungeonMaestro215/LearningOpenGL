#version 330 core
out vec4 FragColor;

in vec3 worldNormal;
in vec3 worldPosition;

uniform vec3 viewPosition;
uniform samplerCube skybox;

void main()
{             
    // Reflection
    vec3 I = normalize(worldPosition - viewPosition);
    vec3 R = reflect(I, normalize(worldNormal));
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}