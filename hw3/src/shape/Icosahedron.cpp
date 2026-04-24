#include "shape/Icosahedron.h"
#include "util/Shader.h"
#include <fstream>

Icosahedron::Icosahedron(Shader * pShader, const std::string & vertexFile, const glm::mat4 & model)
    : Mesh(pShader, model)
{
    if (std::ifstream fin {vertexFile})
    {
        glm::vec3 v1, v2, v3;
        while (fin >> v1.x >> v1.y >> v1.z >> v2.x >> v2.y >> v2.z >> v3.x >> v3.y >> v3.z)
        {
            // For a unit sphere representation, normals = normalized positions
            glm::vec3 n1 = glm::normalize(v1);
            glm::vec3 n2 = glm::normalize(v2);
            glm::vec3 n3 = glm::normalize(v3);

            vertices.emplace_back(v1, n1, kColor);
            vertices.emplace_back(v2, n2, kColor);
            vertices.emplace_back(v3, n3, kColor);

            positions.push_back(v1);
            positions.push_back(v2);
            positions.push_back(v3);
        }
    }
    else
    {
        throw std::runtime_error("failed to open " + vertexFile);
    }
    updateBuffers();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Icosahedron::subdivide() {
    std::vector<glm::vec3> nextPositions;

    for (size_t i = 0; i < positions.size(); i += 3) {
        glm::vec3 v1 = positions[i];
        glm::vec3 v2 = positions[i+1];
        glm::vec3 v3 = positions[i+2];

        glm::vec3 v12 = glm::normalize((v1 + v2) / 2.0f);
        glm::vec3 v23 = glm::normalize((v2 + v3) / 2.0f);
        glm::vec3 v31 = glm::normalize((v3 + v1) / 2.0f);

        // Add 4 triangles to replace the 1 old one
        nextPositions.push_back(v1);  nextPositions.push_back(v12); nextPositions.push_back(v31);
        nextPositions.push_back(v2);  nextPositions.push_back(v23); nextPositions.push_back(v12);
        nextPositions.push_back(v3);  nextPositions.push_back(v31); nextPositions.push_back(v23);
        nextPositions.push_back(v12); nextPositions.push_back(v23); nextPositions.push_back(v31);
    }

    positions = std::move(nextPositions);
    updateBuffers(); // Send new data to GPU
}

void Icosahedron::updateBuffers() {
    vertices.clear();
    for (const auto& pos : positions) {
        vertices.emplace_back(pos, glm::normalize(pos), kColor);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Icosahedron::render(float timeElapsedSinceLastFrame)
{
    pShader->use();
    pShader->setMat4("model", model);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
    glBindVertexArray(0);
}