//--------------------------
// - Tabby -
// Renderer3D Unlit Mesh Shader
// --------------------------

#type vertex
#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;
layout (location = 2) in vec3 a_Normal;

out VertexOutput {
    vec2 TexCoord;
    vec3 Normal;
} Output;

uniform mat4 u_Model;
uniform Camera {
    mat4 u_ViewProjection;
};

void main()
{
    // v_FragPos = vec3(u_Model * a_Position);// World space
    // v_TexCoord = a_TexCoord;

    Output.TexCoord = a_TexCoord;
    Output.Normal = a_Normal;

    gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

in VertexOutput {
    vec2 TexCoord;
    vec3 Normal;
} Input;

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
		vec2 tiledAndOffsetTexCoords = (Input.TexCoord * u_AlbedoMapTiling) + (u_AlbedoMapOffset * 0.01f);
        result *= texture(u_AlbedoSampler, tiledAndOffsetTexCoords);

    }

    color = result;
} 
