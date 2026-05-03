#version 410 core
layout (quads, equal_spacing, ccw) in;

out vec3 ourNormal;
out vec3 ourFragPos;
out vec3 ourColor;

uniform mat4 model, view, projection;
uniform vec3 center, color;
uniform float radius;
uniform float e1; // Vertical exponent
uniform float e2; // Horizontal exponent

const float kPi = 3.1415926535f;

float sign_pow(float val, float p) {
    return sign(val) * pow(abs(val), p);
}

void main() {
    vec4 p = gl_in[0].gl_Position; 

    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // map u to [-pi, pi] for longitude
    float omega = (u - 0.5f) * 2.0f * kPi;
    // map v to [-pi/2, pi/2] for latitude
    float eta = (v - 0.5f) * kPi;

    // parametric position
    float cx = sign_pow(cos(eta), e1);
    float cy = sign_pow(cos(omega), e2);
    float sy = sign_pow(sin(omega), e2);
    float cz = sign_pow(sin(eta), e1);

    // swap Y and Z so it stands upright
    vec3 pos = vec3(
        radius * cx * cy,
        radius * cz,      // Up direction
        radius * cx * sy
    );

    // closed-form exact normal for super-quadrics
    vec3 n = vec3(
        sign_pow(cos(eta), 2.0f - e1) * sign_pow(cos(omega), 2.0f - e2),
        sign_pow(sin(eta), 2.0f - e1),
        sign_pow(cos(eta), 2.0f - e1) * sign_pow(sin(omega), 2.0f - e2)
    );
    n = normalize(n);

    pos += center;
    gl_Position = projection * view * model * vec4(pos, 1.0);
    ourFragPos = vec3(model * vec4(pos, 1.0));
    ourNormal = mat3(transpose(inverse(model))) * n;
    ourColor = color;
}