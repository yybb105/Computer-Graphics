#include <glm/glm.hpp>

#include "shape/Octahedron.h"
#include "util/Shader.h"

#include <fstream>

Octahedron::Octahedron(Shader* pShader, const std::string& vertexFile, const glm::mat4& model)
    : Mesh(pShader, model)
{
    const glm::vec3 octColor{0.2f, 0.6f, 0.8f}; // Different color for distinction

    if (std::ifstream fin{vertexFile}) {
        glm::vec3 v1, v2, v3;
        while (fin >> v1.x >> v1.y >> v1.z >> v2.x >> v2.y >> v2.z >> v3.x >> v3.y >> v3.z)
        {
            // Instead of a shared face normal (fn)...
            // Use the normalized position of each vertex as its own normal!
            glm::vec3 n1 = glm::normalize(v1); 
            glm::vec3 n2 = glm::normalize(v2);
            glm::vec3 n3 = glm::normalize(v3);

            vertices.emplace_back(v1, n1, kColor);
            vertices.emplace_back(v2, n2, kColor);
            vertices.emplace_back(v3, n3, kColor);
        }
    } else {
        throw std::runtime_error("failed to open " + vertexFile);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Octahedron::render(float timeElapsedSinceLastFrame) {
    pShader->use();
    pShader->setMat4("model", model);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0);
}