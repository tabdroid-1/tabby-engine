#version 330 core

// layout (location = 0) in vec3 vertexPosition;
// layout (location = 1) in vec2 vertexTexCoords;
// layout (location = 2) in vec3 vertexNormal;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;
// layout(location = 3) in int a_EntityID;

out vec2 o_TexCoords;
out vec3 o_Position;
out vec3 o_Normal;

uniform mat4 model;
uniform Camera {
    mat4 u_ViewProjection;
};

void main()
{
    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
    o_TexCoords = vec2(a_TexCoord.x, 1.0 - a_TexCoord.y);
    o_Position = (model * vec4(a_Position, 1.0)).xyz;
    o_Normal = mat3(model) * a_Normal;
}