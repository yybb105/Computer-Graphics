#include <glm/glm.hpp>

#include "shape/Cube.h"
#include "util/Shader.h"

#include <fstream>

Cube::Cube(Shader* pShader, const std::string& vertexFile, const glm::mat4& model)
    : Mesh(pShader, model)
{
    if (std::ifstream fin{vertexFile}) {
        glm::vec3 v1, v2, v3;
        while (fin >> v1.x >> v1.y >> v1.z >> v2.x >> v2.y >> v2.z >> v3.x >> v3.y >> v3.z) {
            // Normal calculation (Cross product of two edges)
            glm::vec3 fn = glm::normalize(glm::cross(v2 - v1, v3 - v2));
            vertices.emplace_back(v1, fn, kColor);
            vertices.emplace_back(v2, fn, kColor);
            vertices.emplace_back(v3, fn, kColor);
        }
    } else {
        throw std::runtime_error("failed to open " + vertexFile);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Cube::render(float timeElapsedSinceLastFrame) {
    pShader->use();
    pShader->setMat4("model", model);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0);
}