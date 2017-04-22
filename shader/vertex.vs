#version 330 core

layout (location = 0) in vec3 pos;
out vec4 color;

void main()
{
    gl_Position = vec4(pos.xy, 0.0, 1.0);
    color = vec4(pos.z, pos.z, pos.z, 1.0);
}
