#version 330 core

out vec4 FragColor;
in vec3 Normal;
in vec3 WorldPos;
in vec2 TexCoord;

uniform samplerCube _Skybox;
uniform vec3 _ViewPos;


void main()
{
    vec3 I = normalize(WorldPos - _ViewPos);
    vec3 R = reflect(I, normalize(Normal));
    FragColor = vec4(texture(_Skybox, R).rgb, 1.0);
}

