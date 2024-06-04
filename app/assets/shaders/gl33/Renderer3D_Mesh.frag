#version 330 core

// struct PointLight {
//     vec3 position;
//     vec3 color;
//     float strength;
// };

in vec2 o_TexCoords;
in vec3 o_Position;
in vec3 o_Normal;

uniform sampler2D basicTexture;

out vec4 finalColor;

void main()
{
    //ambient
    vec3 temp = 0.2 * texture(basicTexture, o_TexCoords).rgb;

    finalColor = vec4(temp, 1.0);
}
