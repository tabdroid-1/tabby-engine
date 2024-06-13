#version 330 core

in VertexOutput {
    vec2 TexCoord;
    vec3 Normal;
} Input;

out vec4 color;

uniform vec4 u_AmbientColor;
uniform vec4 u_AlbedoColor;
uniform int u_HasAlbedoTexture;
uniform vec2 u_AlbedoMapOffset;
uniform vec2 u_AlbedoMapTiling;
uniform sampler2D u_AlbedoSampler;

void main()
{ 
	// Ambient
    vec4 result = u_AmbientColor;    
	
	// Diffuse
    if(u_HasAlbedoTexture == 1)
    {
		vec2 tiledAndOffsetTexCoords = (Input.TexCoord * u_AlbedoMapTiling) + (u_AlbedoMapOffset * 0.01f);
        result *= texture2D(u_AlbedoSampler, tiledAndOffsetTexCoords);
    }

    color = result;
} 
