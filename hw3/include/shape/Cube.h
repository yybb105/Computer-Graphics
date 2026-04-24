#ifndef CUBE_H
#define CUBE_H

#include <string>
#include <glm/glm.hpp>
#include "shape/Mesh.h"

class Shader;

class Cube : public Mesh
{
public:
    Cube(Shader * pShader, const std::string & vertexFile, const glm::mat4 & model);

    ~Cube() noexcept override = default;

    void render(float timeElapsedSinceLastFrame) override;

private:
    // A distinct color for the Cube
    static constexpr glm::vec3 kColor {1.0f, 0.4f, 0.4f}; 
};

#endif  // CUBE_H