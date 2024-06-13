#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;
layout (location = 2) in vec3 a_Normal;

out VertexOutput {
    vec2 TexCoord;
    vec3 Normal;
} Output;

uniform Camera {
    mat4 u_ViewProjection;
};

void main()
{
    // v_FragPos = vec3(u_Model * a_Position);// World space
    // v_TexCoord = a_TexCoord;

    Output.TexCoord = a_TexCoord;
    Output.Normal = a_Normal;

    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}
