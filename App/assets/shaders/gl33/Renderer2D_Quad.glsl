//--------------------------
// - Tabby -
// Renderer2D Quad Shader
// --------------------------

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in int a_EntityID;

out VertexOutput {
    vec4 Color;
    vec2 TexCoord;
    float TexIndex;
    float TilingFactor;
} Output;

flat out int v_EntityID;

uniform Camera {
    mat4 u_ViewProjection;
};

void main() {
    Output.Color = a_Color;
    Output.TexCoord = a_TexCoord;
    Output.TexIndex = a_TexIndex;
    Output.TilingFactor = a_TilingFactor;
    v_EntityID = a_EntityID;

    gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

in VertexOutput {
    vec4 Color;
    vec2 TexCoord;
    float TexIndex;
    float TilingFactor;
} Input;

flat in int v_EntityID;

out vec4 color;

uniform sampler2D u_Textures[16];
uniform sampler2D u_NormalTextures[16];

void main() {
    vec4 texColor = Input.Color;

	switch(int(Input.TexIndex))
	{
		case  0: texColor *= texture(u_Textures[ 0], Input.TexCoord * Input.TilingFactor); break;
		case  1: texColor *= texture(u_Textures[ 1], Input.TexCoord * Input.TilingFactor); break;
		case  2: texColor *= texture(u_Textures[ 2], Input.TexCoord * Input.TilingFactor); break;
		case  3: texColor *= texture(u_Textures[ 3], Input.TexCoord * Input.TilingFactor); break;
		case  4: texColor *= texture(u_Textures[ 4], Input.TexCoord * Input.TilingFactor); break;
		case  5: texColor *= texture(u_Textures[ 5], Input.TexCoord * Input.TilingFactor); break;
		case  6: texColor *= texture(u_Textures[ 6], Input.TexCoord * Input.TilingFactor); break;
		case  7: texColor *= texture(u_Textures[ 7], Input.TexCoord * Input.TilingFactor); break;
		case  8: texColor *= texture(u_Textures[ 8], Input.TexCoord * Input.TilingFactor); break;
		case  9: texColor *= texture(u_Textures[ 9], Input.TexCoord * Input.TilingFactor); break;
		case 10: texColor *= texture(u_Textures[10], Input.TexCoord * Input.TilingFactor); break;
		case 11: texColor *= texture(u_Textures[11], Input.TexCoord * Input.TilingFactor); break;
		case 12: texColor *= texture(u_Textures[12], Input.TexCoord * Input.TilingFactor); break;
		case 13: texColor *= texture(u_Textures[13], Input.TexCoord * Input.TilingFactor); break;
		case 14: texColor *= texture(u_Textures[14], Input.TexCoord * Input.TilingFactor); break;
		case 15: texColor *= texture(u_Textures[15], Input.TexCoord * Input.TilingFactor); break;
	}

    if (texColor.a == 0.0)
		discard;

    color = texColor;
}
