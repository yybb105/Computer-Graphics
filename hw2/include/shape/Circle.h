#ifndef CIRCLE_H
#define CIRCLE_H


#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shape/GLShape.h"


class Shader;


// Circle[s] class, this class represents MULTIPLE circles.
class Circle : public Renderable, public GLShape
{
public:
    Circle(Shader * shader, const std::vector<glm::vec3> & parameters, int w, int h);

    ~Circle() noexcept override = default;

    void render(float timeElapsedSinceLastFrame, bool animate) override;

    glm::vec3 getParam();
    glm::vec2 getDir();
    void setDir(glm::vec2 dir);
    
    int kWindowWidth {1000};
    int kWindowHeight {1000};

    bool hollow {false};
    bool boundary {true};

    void trans(glm::vec2 to);

private:
    std::vector<glm::vec3> parameters;
    glm::dvec2 curPos;
    bool reversed {false};
    glm::vec2 dir;
};


#endif  // CIRCLE_H
