#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 fColor;
layout (location = 3) in mat4 localToWorld;

layout (location = 0) out vec4 VertexColor;


void main() {
    VertexColor = fColor;
    gl_Position = vec4(pos, 1) * localToWorld;
}