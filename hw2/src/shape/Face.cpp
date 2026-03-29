#include "shape/Face.h"
#include "app/App.h"
#include "shape/Circle.h"
#include "shape/Triangle.h"

// Face::Face(float r, float xpos, float ypos, float vx, float vy) {
Face::Face(Shader* s1, Shader* s2, std::unique_ptr<Circle> boundary) {
    // initialization
    this->boundary = move(boundary);
    this->boundary->hollow = true;
    glm::vec3 bound_info = this->boundary.get()->getParam();

    float half_radius = bound_info.z / 2.0f;
    float leftcenter = bound_info.x - half_radius;
    float rightcenter = bound_info.x + half_radius;
    this->s1 = s1;
    this->s2 = s2;
    // generate face components
    this->eye1 = std::make_unique<Circle>(
                                    s1,
                                    std::vector<glm::vec3> {
                                            {leftcenter, bound_info.y, bound_info.z / 2.0f}
                                    },
                                    this->boundary.get()->kWindowWidth,this->boundary.get()->kWindowHeight
                            );
    this->eye1->hollow = true;
    this->eye1->boundary = false;

    this->eye2 = std::make_unique<Circle>(
                                    s1,
                                    std::vector<glm::vec3> {
                                            {rightcenter, bound_info.y, bound_info.z / 2.0f}
                                    },
                                    this->boundary.get()->kWindowWidth,this->boundary.get()->kWindowHeight
                            );
    this->eye2->hollow = true;
    this->eye2->boundary = false;

    this->mouth = std::make_unique<Triangle>(s2, std::vector<Triangle::Vertex> {
                            // Vertex coordinate (screen-space coordinate), Vertex color
                            {{bound_info.x, bound_info.y-3*half_radius/2.0f}, {1.0f, 1.0f, 1.0f}},
                            {{bound_info.x + half_radius/2.0f, bound_info.y-half_radius}, {1.0f, 1.0f, 1.0f}},
                            {{bound_info.x - half_radius/2.0f, bound_info.y-half_radius}, {1.0f, 1.0f, 1.0f}},
                    });

}
    
// returns true once the face on current layer is balanced 
bool Face:: bump(){
    bumps++;
    if (bumps > 2) {
        // 3456;78910;
        if (left_next){
            if (face1->bump()){
                left_next = false;
                return false;
            }
        }
        else{
            if (face2->bump()){
                left_next = true;
                return true;
            }
        }
    }
    else if (bumps == 2) {
        this->face2 = std::make_unique<Face>(s1,s2,move(this->eye2));
        left_next = true;
        return true;
    }
    else{
        this->face1 = std::make_unique<Face>(s1,s2,move(this->eye1));
        left_next = false;
        return false;
    }
    return false;
}


void Face::render(float timeElapsedSinceLastFrame, bool animate) {
    // render outermost circle
    this->boundary->render(timeElapsedSinceLastFrame,animate);
    // if right eye of current face is also a face

    
    if (bumps > 1) {
        this->face2->boundary->setDir(this->boundary->getDir());    
        this->face2->render(timeElapsedSinceLastFrame,animate);     // generate (recursively) right eye face
    }
    else{
        this->eye2->setDir(this->boundary->getDir());                     // set velocity of eye equal to the boundary
        this->eye2->render(timeElapsedSinceLastFrame,animate);      // otherwise just render eye circle
    }
    // if left eye of current face is also a face
    if (bumps > 0){
        this->face1->boundary->setDir(this->boundary->getDir());    
        this->face1->render(timeElapsedSinceLastFrame,animate);     // generate (recursively) lect eye face
    }
    else {
        this->eye1->setDir(this->boundary->getDir());     
        this->eye1->render(timeElapsedSinceLastFrame,animate);      // otherwise just render eye circle
    }
    
    this->mouth->trans(this->boundary->getDir()); 
    this->mouth->render(timeElapsedSinceLastFrame,animate);     // render mouth
}
