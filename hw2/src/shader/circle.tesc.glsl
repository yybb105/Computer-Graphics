#version 410 core
layout (vertices = 1) out; // Keep at 1 if your App.cpp sends 1 point

void main()
{
    // Outer levels: The roundness of the edge
    gl_TessLevelOuter[0] = 64.0;
    gl_TessLevelOuter[1] = 64.0;
    gl_TessLevelOuter[2] = 64.0;
    gl_TessLevelOuter[3] = 64.0;

    // Inner levels: The "fill" grid
    gl_TessLevelInner[0] = 64.0;
    gl_TessLevelInner[1] = 64.0;

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}