#version 330 core

out vec4 FragColor;
in vec2 texCoord;

uniform sampler2D albedo;
uniform sampler2D decal;

void main()
{
    FragColor = texture(albedo, texCoord) + texture(decal, texCoord) * 0.4f;
}
