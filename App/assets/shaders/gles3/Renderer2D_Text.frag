#version 300 es
precision mediump float;

in vec4 v_Color;
in vec2 v_TexCoord;
flat in int v_EntityID;

out vec4 o_Color;
out int o_EntityID;

uniform sampler2D u_FontAtlas;

float screenPxRange() {
	const float pxRange = 2.0; // set to distance field's pixel range
    vec2 unitRange = vec2(pxRange) / vec2(textureSize(u_FontAtlas, 0));
    vec2 screenTexSize = vec2(1.0) / fwidth(v_TexCoord);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
    // return 1.0;
}

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_FontAtlas, v_TexCoord).r);
    o_Color = vec4(1.0) * texture(u_FontAtlas, v_TexCoord) * sampled;
}
