#version 410 core
layout (vertices = 1) out;

uniform int uTessLevel; 

void main()
{
    // control the grid size (e.g., 15x15, 30x30)
    gl_TessLevelOuter[0] = uTessLevel;
    gl_TessLevelOuter[1] = uTessLevel;
    gl_TessLevelOuter[2] = uTessLevel;
    gl_TessLevelOuter[3] = uTessLevel;

    gl_TessLevelInner[0] = uTessLevel;
    gl_TessLevelInner[1] = uTessLevel;

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}