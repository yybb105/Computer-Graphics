#ifndef FACE_H
#define FACE_H


#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

#include "shape/GLShape.h"
#include "shape/Circle.h"
#include "shape/Triangle.h"


class Face : public Renderable
{
public:
    // Face(float r, float xpos, float ypos, float vx, float vy);
    Face(Shader* circle_shader, Shader* triangle_shader, std::unique_ptr<Circle> boundary);

    ~Face() noexcept override = default;

    void render(float timeElapsedSinceLastFrame, bool animate) override;

    std::unique_ptr<Circle> boundary {nullptr};
    std::unique_ptr<Circle> eye1 {nullptr};
    std::unique_ptr<Circle> eye2 {nullptr};
    std::unique_ptr<Face> face1 {nullptr};
    std::unique_ptr<Face> face2 {nullptr};
    std::unique_ptr<Triangle> mouth {nullptr};
    Shader* s1;
    Shader* s2;
    bool left_next {true};
    

    int bumps {0};
    float radius;
    glm::vec2 pos;
    glm::vec2 dir;
    std::vector<std::unique_ptr<Renderable>> inside;


    // Simple Constructor
    
    bool bump();
    void angry();
    


    // glm::vec3 getParam();
    // glm::vec2 getDir();
    // void setDir(glm::vec2 dir);

    bool still {false};

private:
    // std::vector<glm::vec3> parameters;
    glm::dvec2 curPos;
    bool reversed {false};
    // glm::vec2 dir;
};


#endif  // CIRCLE_H
