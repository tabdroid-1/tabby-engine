//--------------------------
// - Tabby -
// Renderer2D Quad Shader
// --------------------------

#type vertex
#version 300 es

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in int a_EntityID;

out vec4 v_Color;
out vec2 v_TexCoord;
out float v_TexIndex;
out float v_TilingFactor;
flat out int v_EntityID;

uniform Camera {
    mat4 u_ViewProjection;
};

void main() {
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_TexIndex = a_TexIndex;
    v_TilingFactor = a_TilingFactor;
    v_EntityID = a_EntityID;

    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 300 es
precision mediump float;

in vec4 v_Color;
in vec2 v_TexCoord;
in float v_TexIndex;
in float v_TilingFactor;
flat in int v_EntityID;

out vec4 color;

uniform sampler2D u_Textures[16];

void main() {
    vec4 texColor = v_Color;

	switch(int(v_TexIndex))
	{
		case  0: texColor *= texture(u_Textures[ 0], v_TexCoord * v_TilingFactor); break;
		case  1: texColor *= texture(u_Textures[ 1], v_TexCoord * v_TilingFactor); break;
		case  2: texColor *= texture(u_Textures[ 2], v_TexCoord * v_TilingFactor); break;
		case  3: texColor *= texture(u_Textures[ 3], v_TexCoord * v_TilingFactor); break;
		case  4: texColor *= texture(u_Textures[ 4], v_TexCoord * v_TilingFactor); break;
		case  5: texColor *= texture(u_Textures[ 5], v_TexCoord * v_TilingFactor); break;
		case  6: texColor *= texture(u_Textures[ 6], v_TexCoord * v_TilingFactor); break;
		case  7: texColor *= texture(u_Textures[ 7], v_TexCoord * v_TilingFactor); break;
		case  8: texColor *= texture(u_Textures[ 8], v_TexCoord * v_TilingFactor); break;
		case  9: texColor *= texture(u_Textures[ 9], v_TexCoord * v_TilingFactor); break;
		case 10: texColor *= texture(u_Textures[10], v_TexCoord * v_TilingFactor); break;
		case 11: texColor *= texture(u_Textures[11], v_TexCoord * v_TilingFactor); break;
		case 12: texColor *= texture(u_Textures[12], v_TexCoord * v_TilingFactor); break;
		case 13: texColor *= texture(u_Textures[13], v_TexCoord * v_TilingFactor); break;
		case 14: texColor *= texture(u_Textures[14], v_TexCoord * v_TilingFactor); break;
		case 15: texColor *= texture(u_Textures[15], v_TexCoord * v_TilingFactor); break;
	}

    if (texColor.a == 0.0)
		discard;

    color = texColor;
}
