//--------------------------
// - Tabby -
// Renderer2D Line Shader
// --------------------------

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in int a_EntityID;

uniform Camera {
    mat4 u_ViewProjection;
};

out VertexOutput {
    vec4 Color;
} Output;

flat out int v_EntityID;

void main() {
    Output.Color = a_Color;
    v_EntityID = a_EntityID;

    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

in VertexOutput {
    vec4 Color;
} Input;

flat in int v_EntityID;

out vec4 o_Color;
out int o_EntityID;

void main() {
    o_Color = Input.Color;
    o_EntityID = v_EntityID;
}
