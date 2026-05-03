#ifndef TORUS_H
#define TORUS_H

#include <glm/glm.hpp>
#include "shape/GLShape.h"

class Shader;

class Torus : public Renderable, public GLShape
{
public:
    Torus(
        Shader * pShader, 
        const glm::vec3 & center, 
        float majorRadius, 
        float minorRadius, 
        const glm::vec3 & color, 
        const glm::mat4 & model
    );

    ~Torus() noexcept override = default;

    void render(float timeElapsedSinceLastFrame) override;

    // Functions to handle the subdivision (15 -> 30 -> 60)
    void setResolution(int res) { resolution = res; }
    int getResolution() const { return resolution; }

private:
    static constexpr float kNull {0.0f};

    glm::vec3 center;
    float majorRadius;
    float minorRadius;
    glm::vec3 color;
    
    // Starts at 15 for part (a)
    int resolution {15}; 
};

#endif // TORUS_H