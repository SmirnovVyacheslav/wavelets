#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

uniform float step_x;
uniform float step_y;

in vec4 color[];
out vec4 clr;

void main()
{
    gl_Position = vec4 (gl_in[0].gl_Position);
    clr = vec4(color[0]);
    EmitVertex();

    gl_Position = vec4 (gl_in[0].gl_Position.x + step_x, gl_in[0].gl_Position.yzw);
    clr = vec4(color[0]);
    EmitVertex();

    gl_Position = vec4 (gl_in[0].gl_Position.x + step_x, gl_in[0].gl_Position.y + step_y, gl_in[0].gl_Position.zw);
    clr = vec4(color[0]);
    EmitVertex();

    EndPrimitive();

    gl_Position = vec4 (gl_in[0].gl_Position);
    clr = vec4(color[0]);
    EmitVertex();

    gl_Position = vec4 (gl_in[0].gl_Position.x, gl_in[0].gl_Position.y + step_y, gl_in[0].gl_Position.zw);
    clr = vec4(color[0]);
    EmitVertex();

    gl_Position = vec4 (gl_in[0].gl_Position.x + step_x, gl_in[0].gl_Position.y + step_y, gl_in[0].gl_Position.zw);
    clr = vec4(color[0]);
    EmitVertex();

    EndPrimitive();
}
