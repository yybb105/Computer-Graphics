#version 410 core
layout (triangles, equal_spacing, ccw) in;

out vec3 ourNormal;
out vec3 ourFragPos;
out vec3 ourColor;

uniform mat4 model, view, projection;
uniform vec3 center, color;

void main() {
    // 3 corners of the current triangle patch
    vec3 p0 = gl_in[0].gl_Position.xyz;
    vec3 p1 = gl_in[1].gl_Position.xyz;
    vec3 p2 = gl_in[2].gl_Position.xyz;

    // new point inside the triangle using TessCoords
    vec3 p = gl_TessCoord.x * p0 + gl_TessCoord.y * p1 + gl_TessCoord.z * p2;

    // push the point outwards so the shape becomes rounder as it subdivides
    p = normalize(p); 
    
    vec3 n = p; // normal is just the outward direction
    vec3 pos = p + center;

    gl_Position = projection * view * model * vec4(pos, 1.0);
    ourFragPos = vec3(model * vec4(pos, 1.0));
    ourNormal = mat3(transpose(inverse(model))) * n;
    ourColor = color;
}