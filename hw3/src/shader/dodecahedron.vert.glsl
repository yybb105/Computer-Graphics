#version 410 core
layout (location = 0) in vec3 aPos;

void main() {
    // Pass the actual 3D vertex position to the Tessellation Control Shader
    gl_Position = vec4(aPos, 1.0);
}