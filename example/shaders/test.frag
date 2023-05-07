#version 450 core

in vec4 VertexColor;
out vec4 FragColor;

layout (std140, binding = 1) uniform Uniforms {
    vec4 TestColor;
};

void main() {
    FragColor = VertexColor * TestColor;
}