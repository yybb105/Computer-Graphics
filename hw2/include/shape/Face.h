#ifndef FACE_H
#define FACE_H


#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

#include "shape/GLShape.h"
#include "shape/Circle.h"
#include "shape/Triangle.h"

// Renderable of a face (one circle as face, two circles as eyes, triangle as mouth)
class Face : public Renderable
{
public:
    Face(Shader* circle_shader, Shader* triangle_shader, std::unique_ptr<Circle> boundary);

    ~Face() noexcept override = default;

    void render(float timeElapsedSinceLastFrame, bool animate) override;

    std::unique_ptr<Circle> boundary {nullptr}; // outermost circle; boundary of face
    std::unique_ptr<Circle> eye1 {nullptr}; // left inner circle
    std::unique_ptr<Circle> eye2 {nullptr}; // right inner circle
    std::unique_ptr<Face> face1 {nullptr};  // used when the left eye becomes a face
    std::unique_ptr<Face> face2 {nullptr};  // used when the right eye becomes a face
    std::unique_ptr<Triangle> mouth {nullptr};  // mouth triangle
    Shader* s1;
    Shader* s2;
    bool left_next {true};  // whether the next target for face addition is in the face's left eye
    
    int bumps {0};  // tracks number of bumps with another face
    float radius;
    glm::vec2 pos;
    glm::vec2 dir;
    
    bool bump();    // adds a face

private:
    // parameters;
    glm::dvec2 curPos;
    bool reversed {false};
};


#endif  // CIRCLE_H
