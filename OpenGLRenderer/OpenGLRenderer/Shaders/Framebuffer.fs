#version 330 core

out vec4 FragColor;
in vec2 texCoord;

uniform sampler2D screenTex;

void main()
{
    FragColor = texture(screenTex, texCoord);

    //Gamma correction
    float gamma = 2.2;
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}
