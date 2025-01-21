#version 450

layout(location = 0) in vec5 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = fragColor;
}
