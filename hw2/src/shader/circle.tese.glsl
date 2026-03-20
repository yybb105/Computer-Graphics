#version 410 core
layout (quads, equal_spacing, ccw) in; // Changed to quads!

uniform float innerRadius;
uniform mat3 model;
uniform float windowWidth;
uniform float windowHeight;
const float kPi = 3.14159265358979323846f;

void main()
{
    vec4 params = gl_in[0].gl_Position;

    // 1. Center Position (NDC)
    vec3 localCenter = vec3(2.0f * params.x / windowWidth - 1.0f,
                            2.0f * params.y / windowHeight - 1.0f,
                            1.0f);

    // 2. Apply movement (Translation/Rotation)
    vec3 worldCenter = model * localCenter;
    worldCenter.z = 0.0f;

    // 3. Sizing
    float a = 2.0f * params.z / windowWidth;
    float b = 2.0f * params.z / windowHeight;

    // 4. Quad Math (The Easy Part)
    // u (0 to 1) = angle
    // v (0 to 1) = radius
    float theta = 2.0 * kPi * gl_TessCoord.x;
    float radius = innerRadius + (gl_TessCoord.y * (1.0 - innerRadius));
    
    vec3 offset = vec3(a * cos(theta) * radius, 
                       b * sin(theta) * radius, 
                       0.0f);

    gl_Position = vec4(worldCenter + offset, 1.0f);
}