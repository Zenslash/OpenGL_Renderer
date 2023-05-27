#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0

out vec3 texCoord;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 pos = projection * view * vec4(aPos, 1.0);
    texCoord = aPos;
    gl_Position = pos.xyww;
}
