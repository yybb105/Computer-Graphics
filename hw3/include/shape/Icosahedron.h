#ifndef ICOSAHEDRON_H
#define ICOSAHEDRON_H

#include <string>
#include <glm/glm.hpp>
#include "shape/Mesh.h"

class Shader;

class Icosahedron : public Mesh
{
public:
    Icosahedron(Shader * pShader, const std::string & vertexFile, const glm::mat4 & model);

    ~Icosahedron() noexcept override = default;

    void subdivide();

    void render(float timeElapsedSinceLastFrame) override;

private:
    // Gold or Silver is usually a good color for icosahedrons
    static constexpr glm::vec3 kColor {0.85f, 0.65f, 0.12f}; 
    std::vector<glm::vec3> positions; 
    void updateBuffers();

};

#endif