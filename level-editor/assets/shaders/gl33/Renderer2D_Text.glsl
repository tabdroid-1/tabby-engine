//--------------------------
// - Tabby -
// Renderer2D Text Shader
// --------------------------

#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TexIndex;
layout(location = 4) in int a_EntityID;

uniform Camera
{
	mat4 u_ViewProjection;
};

out VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TexIndex;
} Output;

flat out int v_EntityID;

void main()
{
	Output.Color = a_Color;
	Output.TexCoord = a_TexCoord;
	Output.TexIndex = a_TexIndex;
	v_EntityID = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

out vec4 o_Color;

in VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TexIndex;
} Input;

flat in int v_EntityID;

uniform sampler2D u_FontAtlases[16];

float screenPxRange() {
	const float pxRange = 2.0; // set to distance field's pixel range
    vec2 unitRange;

	switch(int(Input.TexIndex))
	{		
		case   0: unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlases[ 0], 0)); break;
		case   1: unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlases[ 1], 0)); break;
		case   2: unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlases[ 2], 0)); break;
		case   3: unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlases[ 3], 0)); break;
		case   4: unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlases[ 4], 0)); break;
		case   5: unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlases[ 5], 0)); break;
		case   6: unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlases[ 6], 0)); break;
		case   7: unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlases[ 7], 0)); break;
		case   8: unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlases[ 8], 0)); break;
		case   9: unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlases[ 9], 0)); break;
		case  10: unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlases[10], 0)); break;
		case  11: unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlases[11], 0)); break;
		case  12: unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlases[12], 0)); break;
		case  13: unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlases[13], 0)); break;
		case  14: unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlases[14], 0)); break;
		case  15: unitRange = vec2(pxRange)/vec2(textureSize(u_FontAtlases[15], 0)); break;
	}

    vec2 screenTexSize = vec2(1.0)/fwidth(Input.TexCoord);
    return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main()
{
	vec4 texColor;
	vec3 msd;

	switch(int(Input.TexIndex))
	{
		case  0: texColor = Input.Color * texture(u_FontAtlases[ 0], Input.TexCoord); msd = texture(u_FontAtlases[ 0], Input.TexCoord).rgb; break;
		case  1: texColor = Input.Color * texture(u_FontAtlases[ 1], Input.TexCoord); msd = texture(u_FontAtlases[ 1], Input.TexCoord).rgb; break;
		case  2: texColor = Input.Color * texture(u_FontAtlases[ 2], Input.TexCoord); msd = texture(u_FontAtlases[ 2], Input.TexCoord).rgb; break;
		case  3: texColor = Input.Color * texture(u_FontAtlases[ 3], Input.TexCoord); msd = texture(u_FontAtlases[ 3], Input.TexCoord).rgb; break;
		case  4: texColor = Input.Color * texture(u_FontAtlases[ 4], Input.TexCoord); msd = texture(u_FontAtlases[ 4], Input.TexCoord).rgb; break;
		case  5: texColor = Input.Color * texture(u_FontAtlases[ 5], Input.TexCoord); msd = texture(u_FontAtlases[ 5], Input.TexCoord).rgb; break;
		case  6: texColor = Input.Color * texture(u_FontAtlases[ 6], Input.TexCoord); msd = texture(u_FontAtlases[ 6], Input.TexCoord).rgb; break;
		case  7: texColor = Input.Color * texture(u_FontAtlases[ 7], Input.TexCoord); msd = texture(u_FontAtlases[ 7], Input.TexCoord).rgb; break;
		case  8: texColor = Input.Color * texture(u_FontAtlases[ 8], Input.TexCoord); msd = texture(u_FontAtlases[ 8], Input.TexCoord).rgb; break;
		case  9: texColor = Input.Color * texture(u_FontAtlases[ 9], Input.TexCoord); msd = texture(u_FontAtlases[ 9], Input.TexCoord).rgb; break;
		case 10: texColor = Input.Color * texture(u_FontAtlases[10], Input.TexCoord); msd = texture(u_FontAtlases[10], Input.TexCoord).rgb; break;
		case 11: texColor = Input.Color * texture(u_FontAtlases[11], Input.TexCoord); msd = texture(u_FontAtlases[11], Input.TexCoord).rgb; break;
		case 12: texColor = Input.Color * texture(u_FontAtlases[12], Input.TexCoord); msd = texture(u_FontAtlases[12], Input.TexCoord).rgb; break;
		case 13: texColor = Input.Color * texture(u_FontAtlases[13], Input.TexCoord); msd = texture(u_FontAtlases[13], Input.TexCoord).rgb; break;
		case 14: texColor = Input.Color * texture(u_FontAtlases[14], Input.TexCoord); msd = texture(u_FontAtlases[14], Input.TexCoord).rgb; break;
		case 15: texColor = Input.Color * texture(u_FontAtlases[15], Input.TexCoord); msd = texture(u_FontAtlases[15], Input.TexCoord).rgb; break;
	}


    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange()*(sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
	if (opacity == 0.0)
		discard;

	vec4 bgColor = vec4(0.0);
    o_Color = mix(bgColor, Input.Color, opacity);
	if (o_Color.a == 0.0)
		discard;
	
	// o_EntityID = v_EntityID;
}