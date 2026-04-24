#version 410 core

in vec3 ourFragPos;
in vec3 ourNormal;
in vec3 ourColor;

out vec4 fragColor;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;

uniform int uMode;

void main()
{
    vec3 norm;

    if (uMode == 0) {
        // FLAT SHADING: Calculate the normal using the derivative of the position
        // This ensures every pixel on a triangle face has the exact same normal.
        vec3 fdx = dFdx(ourFragPos);
        vec3 fdy = dFdy(ourFragPos);
        norm = normalize(cross(fdx, fdy));
    } else if (uMode == 1){
        // SMOOTH SHADING: Use the interpolated normal from the vertex shader.
        norm = normalize(ourNormal);
    }

    // ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 lightDir = normalize(lightPos - ourFragPos);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - ourFragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 specular = specularStrength * spec * lightColor;

    fragColor = vec4((ambient + diffuse + specular) * ourColor, 1.0f);
}
