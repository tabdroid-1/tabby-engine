#version 300 es
precision mediump float;

in vec2 v_TexCoord;
in vec3 v_Normal;

// uniform sampler2D u_Texture;

out vec4 color;

uniform sampler2D u_AlbedoSampler;
uniform vec4 u_AmbientColor;
uniform vec4 u_AlbedoColor;
uniform int u_HasAlbedoTexture;
uniform vec2 u_AlbedoMapOffset;
uniform vec2 u_AlbedoMapTiling;

void main()
{ 
	// Ambient
    vec4 result = u_AmbientColor;    

	// Diffuse
    if(u_HasAlbedoTexture == 1)
    {
		vec2 tiledAndOffsetTexCoords = (v_TexCoord * u_AlbedoMapTiling) + (u_AlbedoMapOffset * 0.01f);
        result *= texture(u_AlbedoSampler, tiledAndOffsetTexCoords);

    }

    if (result.a == 0.0)
		discard;

    color = result;
} 
