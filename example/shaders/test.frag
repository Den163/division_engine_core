#version 450 core

layout (location = 0) in vec4 VertexColor;
layout (location = 1) in vec2 inUV;

layout (location = 0) out vec4 FragColor;

layout (std140, binding = 1) uniform Uniforms {
    vec4 TestColor;
};

layout (location = 0) uniform sampler2D _tex;

void main() {
    FragColor = VertexColor * TestColor * clamp(texture(_tex, inUV), vec4(1), vec4(1));
}