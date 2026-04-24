#ifndef OCTAHEDRON_H
#define OCTAHEDRON_H

#include <string>
#include <glm/glm.hpp>
#include "shape/Mesh.h"

class Shader;

class Octahedron : public Mesh
{
public:
    Octahedron(Shader * pShader, const std::string & vertexFile, const glm::mat4 & model);

    ~Octahedron() noexcept override = default;

    void render(float timeElapsedSinceLastFrame) override;

private:
    // A distinct color for the Octahedron
    static constexpr glm::vec3 kColor {0.4f, 1.0f, 0.4f};
};

#endif  // OCTAHEDRON_H