#version 410 core
layout (quads, equal_spacing, ccw) in;

out vec3 ourNormal;
out vec3 ourFragPos;
out vec3 ourColor;

uniform mat4 model, view, projection;
uniform vec3 center, color;
uniform float majorRadius, minorRadius;

const float kPi = 3.1415926535f;

void main() {
    // Keep the shader alive
    vec4 p = gl_in[0].gl_Position; 

    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    float theta = u * 2.0 * kPi; // Around the whole donut
    float phi = v * 2.0 * kPi;   // Around the tube

    // 1. Calculate the center of the tube for this slice
    vec3 tubeCenter = vec3(majorRadius * cos(theta), 0.0, majorRadius * sin(theta));

    // 2. Calculate the surface position
    vec3 pos = vec3(
        (majorRadius + minorRadius * cos(phi)) * cos(theta),
        minorRadius * sin(phi),
        (majorRadius + minorRadius * cos(phi)) * sin(theta)
    );

    // 3. Normal is just the direction from the tube center to the surface
    vec3 n = normalize(pos - tubeCenter);

    pos += center;
    gl_Position = projection * view * model * vec4(pos, 1.0);
    ourFragPos = vec3(model * vec4(pos, 1.0));
    ourNormal = mat3(transpose(inverse(model))) * n;
    ourColor = color;
}