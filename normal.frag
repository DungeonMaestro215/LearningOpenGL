#version 330 core

out vec4 FragColor;

in vec3 normal;
in vec3 fragPosition;

uniform mat4 view;

void main()
{
    if (dot(normal, fragPosition) > 0)
        discard;

    FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}  