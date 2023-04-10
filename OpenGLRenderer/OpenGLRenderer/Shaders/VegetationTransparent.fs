#version 330 core

struct Material
{
    vec3 ambient;
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_diffuse3;
    sampler2D texture_specular1;
    sampler2D texture_specular2;
    float shiness;
};

in vec3 Normal;
in vec3 WorldPos;
in vec2 TexCoord;

uniform Material _Material;

out vec4 FragColor;

void main()
{
    vec4 texColor = texture(_Material.texture_diffuse1, TexCoord);
    if(texColor.a < 0.1)
    {
        discard;
    }

    FragColor = texColor;
}

