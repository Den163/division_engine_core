#version 450 core

#define MAX_OBJECTS 9999

layout (location = 0) in vec3 pos;
layout (location = 1) in vec4 fColor;
layout (location = 0) out vec4 VertexColor;

layout (location = 3) in mat4 localToWorld;

void main() {
    VertexColor = fColor;
    gl_Position = vec4(pos, 1) * localToWorld;
}