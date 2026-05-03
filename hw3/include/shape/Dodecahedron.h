#ifndef DODECAHEDRON_H
#define DODECAHEDRON_H

#include <glm/glm.hpp>
#include "shape/GLShape.h"

class Shader;

class Dodecahedron : public Renderable, public GLShape
{
public:
    Dodecahedron(
        Shader * pShader, 
        const glm::vec3 & center, 
        float scale, 
        const glm::vec3 & color, 
        const glm::mat4 & model
    );

    ~Dodecahedron() noexcept override;

    void render(float timeElapsedSinceLastFrame) override;

    // Functions to handle the subdivision levels
    void setResolution(int res) { resolution = res; }
    int getResolution() const { return resolution; }

private:
    glm::vec3 center;
    float scale;
    glm::vec3 color;
    
    int resolution {1}; // Starts at base level 1
    
    // We need an Element Buffer Object (EBO) because we are using indices
    unsigned int ebo; 
};

#endif // DODECAHEDRON_H