#include "shape/Cylinder.h"
#include "util/Shader.h"

Cylinder::Cylinder(Shader * pShader, const glm::vec3 & center, float radius, float height, const glm::vec3 & color, const glm::mat4 & model)
    : GLShape(pShader, model), center(center), radius(radius), height(height), color(color) 
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
float dummyData[3] = {0.0f, 0.0f, 0.0f};
glBufferData(GL_ARRAY_BUFFER, sizeof(dummyData), dummyData, GL_STATIC_DRAW);
}

void Cylinder::render(float timeElapsedSinceLastFrame) {
    // std::cout<<"render cylinder " << radius << " " <<height<<std::endl;
    pShader->use();
    pShader->setMat4("model", model);
    pShader->setVec3("center", center);
    pShader->setFloat("radius", radius);
    pShader->setFloat("height", height); // New uniform!
    pShader->setVec3("color", color);

    glBindVertexArray(vao);
    glPatchParameteri(GL_PATCH_VERTICES, 1);
    glDrawArrays(GL_PATCHES, 0, 3); // 3 Patches to make it "Closed"
}