#version 330 core

out vec4 FragColor;

uniform vec3 _LightColor;

void main()
{
    FragColor = vec4(_LightColor, 1.0f);
}
