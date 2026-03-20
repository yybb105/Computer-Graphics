#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "app/App.h"
#include "shape/Circle.h"
#include "shape/Triangle.h"
#include "shape/Face.h"
#include "util/Shader.h"
#include "fstream"
#include "iostream"
#include "string"


App & App::getInstance()
{
    static App instance;
    return instance;
}


void App::run()
{
    while (!glfwWindowShouldClose(pWindow))
    {
        // Per-frame logic
        perFrameTimeLogic(pWindow);
        processKeyInput(pWindow);

        // Send render commands to OpenGL server
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        stopMovement(pWindow);

        render();

        // Check and call events and swap the buffers
        glfwSwapBuffers(pWindow);
        glfwPollEvents();
    }
}

void App::stopMovement(GLFWwindow * window)
{
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));


    for (int i = 0; i < app.shapes.size() && app.shapes.size() >= 1; i++){
        for (int j = i+1; j < app.shapes.size(); j++){

            glm::vec3 info_i;
            glm::vec3 info_j;

            if (app.mode == 1){
                Circle* c1 = dynamic_cast<Circle*>(app.shapes[i].get());
                Circle* c2 = dynamic_cast<Circle*>(app.shapes[j].get());
                info_i = c1->getParam();
                info_j = c2->getParam();
            }
            if (app.mode == 3){
                Face* f1 = dynamic_cast<Face*>(app.shapes[i].get());
                Face* f2 = dynamic_cast<Face*>(app.shapes[j].get());
                info_i = f1->boundary->getParam();
                info_j = f2->boundary->getParam();
            }
            // glm::vec3 info_i = (dynamic_cast<Circle*>(app.shapes[i].get()))->getParam();
            // glm::vec3 info_j = (dynamic_cast<Circle*>(app.shapes[j].get()))->getParam();
            
                glm::vec2 pos1 = glm::vec2(info_i);
                glm::vec2 pos2 = glm::vec2(info_j);
                // glm::vec2 pos2 = dynamic_cast<Circle*>(app.shapes[j].get())->curPos;
                glm::vec2 delta = pos2 - pos1;
                float distanceSquared = glm::dot(delta, delta); 
                float radiusSum = info_i.z + info_j.z;
    
                if (distanceSquared <= radiusSum * radiusSum){
                    Circle* circ_i;
                    Circle* circ_j;
                    if (app.mode == 1){
                    // std::cout<<"collision between " << i << " and " << j<<std::endl;
                        circ_i = dynamic_cast<Circle*>(app.shapes[i].get());
                        circ_j = dynamic_cast<Circle*>(app.shapes[j].get());
                    }
                    if (app.mode == 3){
                        circ_i = dynamic_cast<Circle*>(dynamic_cast<Face*>(app.shapes[i].get())->boundary.get());
                        circ_j = dynamic_cast<Circle*>(dynamic_cast<Face*>(app.shapes[j].get())->boundary.get());
                        

                    }
                    glm::vec2 temp1 = circ_i->getDir();
                    glm::vec2 temp2 = circ_j->getDir();
                    circ_i->setDir(temp2);
                    circ_j->setDir(temp1);
                    if (app.mode == 3){
                        Face* f1 = dynamic_cast<Face*>(app.shapes[i].get());
                        Face* f2 = dynamic_cast<Face*>(app.shapes[j].get());
                        f1->bump();
                        f2->bump();
                    }
                    // dynamic_cast<Circle*>(app.shapes[j].get())->setDir(dynamic_cast<Circle*>(app.shapes[i].get())->getDir());
                }
            
        }
    }
}

void App::cursorPosCallback(GLFWwindow * window, double xpos, double ypos)
{
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));

    app.mousePos.x = xpos;
    app.mousePos.y = App::kWindowHeight - ypos;

    if (app.mousePressed)
    {
        // // Note: Must calculate offset first, then update lastMouseLeftPressPos.
        // glm::dvec2 offset = app.mousePos - app.lastMouseLeftPressPos;
        app.lastMouseLeftPressPos = app.mousePos;
    }
}


void App::framebufferSizeCallback(GLFWwindow * window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void App::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));

    if (key == GLFW_KEY_A && action == GLFW_RELEASE)
    {
        app.animationEnabled = !app.animationEnabled;
    }
    if (key == GLFW_KEY_1 && action == GLFW_RELEASE){
        app.shapes.clear();
        app.mode = 1;
    }
    if (key == GLFW_KEY_3 && action == GLFW_RELEASE){
        app.shapes.clear();
        app.mode = 3;
    }
}


void App::mouseButtonCallback(GLFWwindow * window, int button, int action, int mods)
{
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));

    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            app.mousePressed = true;
            app.lastMouseLeftClickPos = app.mousePos;
            app.lastMouseLeftPressPos = app.mousePos;
        }
        else if (action == GLFW_RELEASE)
        {
            app.mousePressed = false;

            std::cout<<"mouseb released."<<std::endl;

            switch(app.mode){

                case 1:
                {
                // read radius from config file
                std::ifstream file("etc/config.txt");
                float r;
                file >> r;
                    bool accept = (r>0) && (app.lastMouseLeftClickPos.x + r >= 0) && (app.lastMouseLeftClickPos.x + r <= app.kWindowWidth) && (app.lastMouseLeftClickPos.y + r >= 0) && (app.lastMouseLeftClickPos.y + r <= app.kWindowHeight);
                for (int i = 0; i < app.shapes.size(); i++){
                    glm::vec3 info_i = (dynamic_cast<Circle*>(app.shapes[i].get()))->getParam();
                    glm::vec2 pos2 = glm::vec2(info_i);
                    glm::vec2 delta = pos2 - glm::vec2(app.lastMouseLeftClickPos);
                    float distanceSquared = glm::dot(delta, delta); 
                    float radiusSum = info_i.z + r;
    
                if (distanceSquared <= radiusSum * radiusSum)
                    accept = false;
                }
                if (accept){
                    app.shapes.emplace_back(
                            std::make_unique<Circle>(
                                    app.pCircleShader.get(),
                                    std::vector<glm::vec3> {
                                            // Coordinate (x, y) of the center and the radius (screen-space)
                                            {app.lastMouseLeftClickPos.x, app.lastMouseLeftClickPos.y, r}
                                            // {800.0f, 326.8f, 30.0f},
                                            // {500.0f, 846.4f, 400.0f}
                                    },app.kWindowWidth,app.kWindowHeight
                            )
                    );
                    // (dynamic_cast<Circle*>(app.shapes.back().get()))->setDir();
                }
                break;
                }
                case 3:
                {
                    // read radius from config file
                    std::ifstream file("etc/config.txt");
                    float r;
                    file >> r;
                    bool accept = (r>0) && (app.lastMouseLeftClickPos.x + r >= 0) && (app.lastMouseLeftClickPos.x + r <= app.kWindowWidth) && (app.lastMouseLeftClickPos.y + r >= 0) && (app.lastMouseLeftClickPos.y + r <= app.kWindowHeight);
                    for (int i = 0; i < app.shapes.size(); i++){
                        glm::vec3 info_i = (dynamic_cast<Circle*>((dynamic_cast<Face*>(app.shapes[i].get()))->boundary.get()))->getParam();
                        glm::vec2 pos2 = glm::vec2(info_i);
                        glm::vec2 delta = pos2 - glm::vec2(app.lastMouseLeftClickPos);
                        float distanceSquared = glm::dot(delta, delta); 
                        float radiusSum = info_i.z + r;
        
                        if (distanceSquared <= radiusSum * radiusSum){
                            accept = false;
                            dynamic_cast<Face*>(app.shapes[i].get());
                        }
                    }
                // // read radius from config file
                // std::ifstream file("etc/config.txt");
                // float r;
                // file >> r;
                // bool accept = (r>0) && (app.lastMouseLeftClickPos.x + r >= 0) && (app.lastMouseLeftClickPos.x + r <= 1000) && (app.lastMouseLeftClickPos.y + r >= 0) && (app.lastMouseLeftClickPos.y + r <= 1000);
                // for (int i = 0; i < app.shapes.size(); i++){
                //     glm::vec3 info_i = (dynamic_cast<Circle*>(app.shapes[i].get()))->getParam();
                //     glm::vec2 pos2 = glm::vec2(info_i);
                //     glm::vec2 delta = pos2 - glm::vec2(app.lastMouseLeftClickPos);
                //     float distanceSquared = glm::dot(delta, delta); 
                //     float radiusSum = info_i.z + r;
    
                // if (distanceSquared <= radiusSum * radiusSum)
                //     accept = false;
                // }
                // if (accept){
                if (accept){
                auto c = std::make_unique<Circle>(
                                    app.pCircleShader.get(),
                                    std::vector<glm::vec3> {
                                            {app.lastMouseLeftClickPos.x, app.lastMouseLeftClickPos.y, r}
                                    },app.kWindowWidth,app.kWindowHeight
                            );

                    app.shapes.emplace_back(
                            std::make_unique<Face>(
                                    app.pCircleShader.get(),app.pTriangleShader.get(),
                                std::move(c)        
                            )
                    );
                //     // (dynamic_cast<Circle*>(app.shapes.back().get()))->setDir();
                // }
                }

                break;
                }
                break;
            }

            #ifdef DEBUG_MOUSE_POS
            std::cout << "[ " << app.mousePos.x << ' ' << app.mousePos.y << " ]\n";
            #endif
        }
    }
}


void App::scrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{

}


void App::perFrameTimeLogic(GLFWwindow * window)
{
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));

    double currentFrame = glfwGetTime();
    app.timeElapsedSinceLastFrame = currentFrame - app.lastFrameTimeStamp;
    app.lastFrameTimeStamp = currentFrame;
}


void App::processKeyInput(GLFWwindow * window)
{

}


App::App() : Window(kWindowWidth, kWindowHeight, kWindowName, nullptr, nullptr)
{
    // GLFW boilerplate.
    glfwSetWindowUserPointer(pWindow, this);
    glfwSetCursorPosCallback(pWindow, cursorPosCallback);
    glfwSetFramebufferSizeCallback(pWindow, framebufferSizeCallback);
    glfwSetKeyCallback(pWindow, keyCallback);
    glfwSetMouseButtonCallback(pWindow, mouseButtonCallback);
    glfwSetScrollCallback(pWindow, scrollCallback);

    // Global OpenGL pipeline settings
    glViewport(0, 0, kWindowWidth, kWindowHeight);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(1.0f);
    glPointSize(1.0f);

    // Initialize shaders and objects-to-render;
    pTriangleShader = std::make_unique<Shader>("src/shader/triangle.vert.glsl",
                                               "src/shader/triangle.frag.glsl");
    pCircleShader = std::make_unique<Shader>("src/shader/circle.vert.glsl",
                                             "src/shader/circle.tesc.glsl",
                                             "src/shader/circle.tese.glsl",
                                             "src/shader/circle.frag.glsl");

//     shapes.emplace_back(
//             std::make_unique<Triangle>(
//                     pTriangleShader.get(),
//                     std::vector<Triangle::Vertex> {
//                             // Vertex coordinate (screen-space coordinate), Vertex color
//                             {{200.0f, 326.8f}, {1.0f, 0.0f, 0.0f}},
//                             {{800.0f, 326.8f}, {0.0f, 1.0f, 0.0f}},
//                             {{500.0f, 846.4f}, {0.0f, 0.0f, 1.0f}},
//                     }
//             )
//     );

//     shapes.emplace_back(
//             std::make_unique<Circle>(
//                     pCircleShader.get(),
//                     std::vector<glm::vec3> {
//                             // Coordinate (x, y) of the center and the radius (screen-space)
//                             {200.0f, 326.8f, 200.0f},
//                             {800.0f, 326.8f, 300.0f},
//                             {500.0f, 846.4f, 400.0f}
//                     }
//             )
//     );
}


void App::render()
{
    auto t = static_cast<float>(timeElapsedSinceLastFrame);

    // Update all shader uniforms.
    pTriangleShader->use();
    pTriangleShader->setFloat("windowWidth", kWindowWidth);
    pTriangleShader->setFloat("windowHeight", kWindowHeight);

    pCircleShader->use();
    pCircleShader->setFloat("windowWidth", kWindowWidth);
    pCircleShader->setFloat("windowHeight", kWindowHeight);

    // Render all shapes.
    for (auto & s : shapes)
    {
        s->render(t, animationEnabled);
    }
}
