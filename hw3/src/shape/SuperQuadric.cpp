#include "shape/SuperQuadric.h"
#include "util/Shader.h"

SuperQuadric::SuperQuadric(
    Shader * pShader, 
    const glm::vec3 & center, 
    float radius, 
    float e1, 
    float e2, 
    const glm::vec3 & color, 
    const glm::mat4 & model
)
    : GLShape(pShader, model), 
      center(center), 
      radius(radius), 
      e1(e1), 
      e2(e2), 
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

void SuperQuadric::render(float timeElapsedSinceLastFrame)
{
    pShader->use();
    pShader->setMat4("model", model);
    pShader->setVec3("center", center);
    
    // superQuadric specific
    pShader->setFloat("radius", radius);
    pShader->setFloat("e1", e1); 
    pShader->setFloat("e2", e2); 
    pShader->setVec3("color", color);
    
    // send the subdivision level to the TESC
    pShader->setInt("uTessLevel", resolution); 

    glBindVertexArray(vao);
    
    // draw single patch
    glPatchParameteri(GL_PATCH_VERTICES, 1);
    glDrawArrays(GL_PATCHES, 0, 1);

    glBindVertexArray(0);
}