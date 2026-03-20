#include <glm/glm.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>

#include "shape/Circle.h"
#include "util/Shader.h"


Circle::Circle(Shader * shader, const std::vector<glm::vec3> & parameters, int w, int h) : GLShape(shader), parameters(parameters)
{
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Vertex coordinate attribute array "layout (position = 0) in vec3 aPos"
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,                             // index: corresponds to "0" in "layout (position = 0)"
                          3,                             // size: each "vec3" generic vertex attribute has 3 values
                          GL_FLOAT,                      // data type: "vec3" generic vertex attributes are GL_FLOAT
                          GL_FALSE,                      // do not normalize data
                          sizeof(glm::vec3),             // stride between attributes in VBO data
                          reinterpret_cast<void *>(0));  // offset of 1st attribute in VBO data

    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizei>(parameters.size() * sizeof(glm::vec3)),
                 parameters.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    curPos.x = parameters[0].x;
    curPos.y = parameters[0].y;

    // read velocity from config
    std::ifstream file("etc/config.txt");
    int ignore;
    file >> ignore;
    file >> this->dir.x;
    file >> this->dir.y;
    this->kWindowWidth = w;
    this->kWindowHeight = h;
    // std::cout<<"(" << this->dir.x << ", " << this->dir.y << ")"<< std::endl;
}

glm::vec2 Circle::getDir(){
    return (reversed)?(-this->dir):(this->dir);
}

void Circle::setDir(glm::vec2 d){
    // std::cout << "set " << (this->dir).x << (this->dir).y <<" to "<< d.x << d.y<<std::endl;
    reversed = false;
    this->dir = d;
}

glm::vec3 Circle::getParam(){
    glm::vec3 params;
    params.x = curPos.x;
    params.y = curPos.y;
    params.z = parameters[0].z;

    return params;
}

void Circle::trans(glm::vec2 to){
            model = glm::translate(model,to);
}

void Circle::render(float timeElapsedSinceLastFrame, bool animate)
{
    if (animate & boundary)
    {
        // std::cout<< "(" << curPos.x<<", " << curPos.y<<")"<<std::endl;    
        if (curPos.x - parameters[0].z <= 0 || curPos.x + parameters[0].z >= this->kWindowWidth || curPos.y + parameters[0].z >= this->kWindowHeight || curPos.y - parameters[0].z <= 0){
            reversed = !(reversed);
        }
        if (reversed){
            model = glm::translate(model, -this->dir);
            curPos.x -= dir.x * (this->kWindowWidth / 2.0f);
            curPos.y -= dir.y * (this->kWindowHeight / 2.0f);
        }
        else{
            model = glm::translate(model, this->dir);
            curPos.x += dir.x * (this->kWindowWidth / 2.0f);
            curPos.y += dir.y * (this->kWindowHeight / 2.0f);
        }
    }
    else if (!boundary)
    {
        model = glm::translate(model, this->dir);
            curPos.x += dir.x * (this->kWindowHeight / 2.0f);
            curPos.y += dir.y * (this->kWindowHeight / 2.0f);
    }

    pShader->use();
    pShader->setMat3("model", model);
    if (hollow)
        pShader->setFloat("innerRadius", .98f);
    else
        pShader->setFloat("innerRadius", 0.0f);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glPatchParameteri(GL_PATCH_VERTICES, 1);
    glDrawArrays(GL_PATCHES,
                 0,                                          // start from index 0 in current VBO
                 static_cast<GLsizei>(parameters.size()));  // draw these number of elements

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
