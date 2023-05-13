#version 450 core

layout (location = 0) in vec4 VertexColor;
layout (location = 0) out vec4 FragColor;

layout (std140, binding = 1) uniform Uniforms {
    vec4 TestColor;
};

void main() {
    FragColor = VertexColor * TestColor;
}