#version 410 core
layout (quads, equal_spacing, ccw) in;

out vec3 ourNormal;
out vec3 ourFragPos;
out vec3 ourColor;

uniform mat4 model, view, projection;
uniform vec3 center, color;
uniform float radius, height;

const float kPi = 3.1415926535f;

void main() {

    
    vec4 WC = gl_in[0].gl_Position;

    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float phi = 2.0 * kPi * u;
    
    vec3 pos;
    vec3 n;

    if (gl_PrimitiveID == 0) { // THE TUBE
        pos = vec3(radius * cos(phi), (v - 0.5) * height, radius * sin(phi));
        n = vec3(cos(phi), 0.0, sin(phi)); // Normal points out from center axis
    } 
    else if (gl_PrimitiveID == 1) { // TOP CAP
        pos = vec3(v * radius * cos(phi), 0.5 * height, v * radius * sin(phi));
        n = vec3(0.0, 1.0, 0.0); // Points straight up
    } 
    else { // BOTTOM CAP
        pos = vec3(v * radius * cos(phi), -0.5 * height, v * radius * sin(phi));
        n = vec3(0.0, -1.0, 0.0); // Points straight down
    }

    pos += center;
    gl_Position = projection * view * model * vec4(pos, 1.0);
    ourFragPos = vec3(model * vec4(pos, 1.0));
    ourNormal = mat3(transpose(inverse(model))) * n;
    ourColor = color;
    
}