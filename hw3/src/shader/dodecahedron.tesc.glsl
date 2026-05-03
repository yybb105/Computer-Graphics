#version 410 core
layout (vertices = 3) out;

uniform int uTessLevel;

void main() {
    // vertex positions
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    
    // subdivision level for the triangle
    if (gl_InvocationID == 0) {
        gl_TessLevelOuter[0] = uTessLevel;
        gl_TessLevelOuter[1] = uTessLevel;
        gl_TessLevelOuter[2] = uTessLevel;
        gl_TessLevelInner[0] = uTessLevel;
    }
}