#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 TexCoord[];
//in vec3 Normal[];
in vec3 FragPosition[];
in vec4 FragPositionLightSpace[];

in vec3 Normal[];
in vec3 WorldNormal[];
in vec3 WorldPosition[];

out vec3 vertNormal;

out vec2 texCoord;
out vec3 normal;
out vec3 fragPosition;
out vec3 worldNormal;
out vec3 worldPosition;
out vec4 fragPositionLightSpace;

//uniform mat4 model;
//uniform mat4 view;
uniform mat4 projection;
uniform float time;

vec3 GetNormal();
vec4 explode(vec4 position, vec3 normal);

void main() {
    normal = GetNormal();

    vec4 explodePosition;

    explodePosition = explode(gl_in[0].gl_Position, normal);
    fragPosition = explodePosition.xyz;
    gl_Position = projection * explodePosition;
    //TexCoords = gs_in[0].texCoords;
    texCoord = TexCoord[0];
    vertNormal = Normal[0];
    worldNormal = WorldNormal[0];
    worldPosition = explode(vec4(WorldPosition[0], 1.0), worldNormal).xyz;
    fragPositionLightSpace = FragPositionLightSpace[0];
    EmitVertex();

    explodePosition = explode(gl_in[1].gl_Position, normal);
    fragPosition = explodePosition.xyz;
    gl_Position = projection * explodePosition;
    //TexCoords = gs_in[1].texCoords;
    texCoord = TexCoord[1];
    vertNormal = Normal[1];
    worldNormal = WorldNormal[1];
    worldPosition = explode(vec4(WorldPosition[1], 1.0), worldNormal).xyz;
    fragPositionLightSpace = FragPositionLightSpace[1];
    EmitVertex();

    explodePosition = explode(gl_in[2].gl_Position, normal);
    fragPosition = explodePosition.xyz;
    gl_Position = projection * explodePosition;
    //TexCoords = gs_in[2].texCoords;
    texCoord = TexCoord[2];
    vertNormal = Normal[2];
    worldNormal = WorldNormal[2];
    worldPosition = explode(vec4(WorldPosition[2], 1.0), worldNormal).xyz;
    fragPositionLightSpace = FragPositionLightSpace[2];
    EmitVertex();

    EndPrimitive();
}

vec3 GetNormal() {
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return -normalize(cross(a, b));
}  

vec4 explode(vec4 position, vec3 normal) {
    float magnitude = 2.0f;
    vec3 direction = normal * ((sin(time) + 1.0f) / 2.0f) * magnitude; 
    return position + vec4(direction, 0.0f);
} 
