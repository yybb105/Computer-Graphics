#include "shape/Dodecahedron.h"
#include "util/Shader.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
Dodecahedron::Dodecahedron(
    Shader * pShader, 
    const glm::vec3 & center, 
    float scale, 
    const glm::vec3 & color, 
    const glm::mat4 & model
)
    : GLShape(pShader, model), 
      center(center), 
      scale(scale), 
      color(color)
{
    // Golden Ratio and its inverse
    const float a = 1.0f;
    const float b = 1.0f / 1.61803398875f;
    const float c = 1.61803398875f;

    // 20 vertices of a Dodecahedron
    float vertices[] = {
         a,  a,  a,    a,  a, -a,    a, -a,  a,    a, -a, -a, // 0, 1, 2, 3

        -a,  a,  a,   -a,  a, -a,   -a, -a,  a,   -a, -a, -a, // 4, 5, 6, 7
        
         0,  b,  c,    0,  b, -c,    0, -b,  c,    0, -b, -c, // 8, 9, 10, 11
        
         b,  c,  0,    b, -c,  0,   -b,  c,  0,   -b, -c,  0, // 12, 13, 14, 15
        
         c,  0,  b,    c,  0, -b,   -c,  0,  b,   -c,  0, -b  // 16, 17, 18, 19
    };

    // 12 pentagonal faces broken down into 36 triangles (108 indices total)
    unsigned int indices[] = {
        0, 16, 2,   0, 2, 10,   0, 10, 8,   // Face 1
        0, 8, 4,    0, 4, 14,   0, 14, 12,  // Face 2
        16, 17, 1,  16, 1, 12,  16, 12, 0,  // Face 3
        1, 9, 11,   1, 11, 3,   1, 3, 17,   // Face 4
        1, 12, 14,  1, 14, 5,   1, 5, 9,    // Face 5
        2, 13, 15,  2, 15, 6,   2, 6, 10,   // Face 6
        13, 3, 11,  13, 11, 7,  13, 7, 15,  // Face 7
        3, 17, 16,  3, 16, 2,   3, 2, 13,   // Face 8
        4, 8, 10,   4, 10, 6,   4, 6, 18,   // Face 9
        14, 4, 18,  14, 18, 19, 14, 19, 5,  // Face 10
        5, 19, 7,   5, 7, 11,   5, 11, 9,   // Face 11
        15, 7, 19,  15, 19, 18, 15, 18, 6   // Face 12
    };

    // Set up OpenGL buffers
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    // Load Vertices into VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Load Indices into EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position Attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
}

Dodecahedron::~Dodecahedron() noexcept {
    glDeleteBuffers(1, &ebo);
}

void Dodecahedron::render(float timeElapsedSinceLastFrame)
{
    pShader->use();

    // Scale the base mesh if necessary
    glm::mat4 scaledModel = glm::scale(model, glm::vec3(scale));
    
    pShader->setMat4("model", scaledModel);
    pShader->setVec3("center", center);
    pShader->setVec3("color", color);
    
    // Send the current subdivision level to the TESC shader
    pShader->setInt("uTessLevel", resolution); 

    glBindVertexArray(vao);
    
    // We send triangles (3 vertices per patch) to the Tessellation Pipeline
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    
    // Draw using our Element Buffer
    glDrawElements(GL_PATCHES, 108, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}