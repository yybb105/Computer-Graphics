#ifndef SUPERQUADRIC_H
#define SUPERQUADRIC_H

#include <glm/glm.hpp>
#include "shape/GLShape.h"

class Shader;

class SuperQuadric : public Renderable, public GLShape
{
public:
    SuperQuadric(
        Shader * pShader, 
        const glm::vec3 & center, 
        float radius, 
        float e1, 
        float e2, 
        const glm::vec3 & color, 
        const glm::mat4 & model
    );

    ~SuperQuadric() noexcept override = default;

    void render(float timeElapsedSinceLastFrame) override;

    // Optional: Let's keep the subdivision logic just in case you want to make it smoother!
    void setResolution(int res) { resolution = res; }
    int getResolution() const { return resolution; }

private:
    static constexpr float kNull {0.0f};

    glm::vec3 center;
    float radius;
    float e1;
    float e2;
    glm::vec3 color;
    
    // Default high resolution since super-quadrics need lots of polygons for sharp corners
    int resolution {64}; 
};

#endif // SUPERQUADRIC_H