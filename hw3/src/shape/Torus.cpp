#include "shape/Torus.h"
#include "util/Shader.h"

Torus::Torus(
    Shader * pShader, 
    const glm::vec3 & center, 
    float majorRadius, 
    float minorRadius, 
    const glm::vec3 & color, 
    const glm::mat4 & model
)
    : GLShape(pShader, model), 
      center(center), 
      majorRadius(majorRadius), 
      minorRadius(minorRadius), 
      color(color)
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);


    float dummyData[1] = {0.0f};
    glBufferData(GL_ARRAY_BUFFER, sizeof(dummyData), dummyData, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Torus::render(float timeElapsedSinceLastFrame)
{
    pShader->use();
    pShader->setMat4("model", model);
    pShader->setVec3("center", center);
    
    pShader->setFloat("majorRadius", majorRadius);
    pShader->setFloat("minorRadius", minorRadius);
    pShader->setVec3("color", color);
    
    pShader->setInt("uTessLevel", resolution); 

    glBindVertexArray(vao);
    
    // draw our single patch
    glPatchParameteri(GL_PATCH_VERTICES, 1);
    glDrawArrays(GL_PATCHES, 0, 1);

    glBindVertexArray(0);
}