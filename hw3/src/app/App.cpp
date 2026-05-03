#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "app/App.h"
#include "shape/Line.h"
#include "shape/Mesh.h"
#include "shape/Sphere.h"
#include "shape/Tetrahedron.h"
#include "shape/Octahedron.h"
#include "shape/Icosahedron.h"
#include "shape/Cube.h"
#include "shape/Cone.h"
#include "shape/Cylinder.h"
#include "shape/Torus.h"
#include "shape/Dodecahedron.h"
#include "shape/SuperQuadric.h"
#include "util/Shader.h"


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
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render();

        // Check and call events and swap the buffers
        glfwSwapBuffers(pWindow);
        glfwPollEvents();
    }
}


void App::cursorPosCallback(GLFWwindow * window, double xpos, double ypos)
{
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));

    app.mousePos.x = xpos;
    app.mousePos.y = App::kWindowHeight - ypos;

    if (app.mousePressed)
    {
        // Note: Must calculate offset first, then update lastMouseLeftPressPos.
        glm::dvec2 offset = app.mousePos - app.lastMouseLeftPressPos;
        app.lastMouseLeftPressPos = app.mousePos;
        app.camera.processMouseMovement(offset.x, offset.y);
    }
}


void App::framebufferSizeCallback(GLFWwindow * window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void App::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
    if (action == GLFW_RELEASE)
    {
        switch(key){
            case GLFW_KEY_1:    
            {
                if (app.shapes.size() > 1)
                    app.shapes.erase(app.shapes.begin() + 1, app.shapes.end());

                // place tetrahedron
                app.shapes.emplace_back(
                std::make_unique<Tetrahedron>(
                        app.pMeshShader.get(),
                        "var/tetrahedron.txt",
                        glm::translate(glm::mat4(1.0f), {-2.0f, 0.0f, 0.0f})
                )
                );

                // place cube
                app.shapes.emplace_back(
                std::make_unique<Cube>(
                    app.pMeshShader.get(),
                    "var/cube.txt",
                    glm::translate(glm::mat4(1.0f), {0.0f, 0.0f, 0.0f}) // Center it
                )
                );

                // place octahedron
                app.shapes.emplace_back(
                std::make_unique<Octahedron>(
                    app.pMeshShader.get(),
                    "var/octahedron.txt",
                    glm::translate(glm::mat4(1.0f), {2.0f, 0.0f, 0.0f}) // Move it to the right
                )
                );
                app.mode = 1;
                break;
            }
            case GLFW_KEY_F1:
                // wireframe
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                app.displayMode = 1;
                break;
            case GLFW_KEY_F2:
                // flat
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                app.displayMode = 0;
                break;
            // case GLFW_KEY_F3:
            //     // normal
            //     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            //     app.displayMode = 3;
            //     break;
            case GLFW_KEY_F4:
                // smooth
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                app.displayMode = 1;
                break;
            case GLFW_KEY_X:
                // display axes
                app.axes = !app.axes;
                break;
            case GLFW_KEY_2:
            {
                app.mode = 2;
                app.displayMode=0;
                if (app.shapes.size() > 1)
                    app.shapes.erase(app.shapes.begin() + 1, app.shapes.end());

                // add icosahedron
                app.shapes.emplace_back(
                    std::make_unique<Icosahedron>(
                        app.pMeshShader.get(),         
                        "var/icosahedron.txt",      
                        glm::translate(glm::mat4(1.0f), {0.0f, 0.0f, 0.0f}) // Position it to the right
                    )
                );
                break;
            }
            case GLFW_KEY_3:
            {
                app.mode = 3;
                app.displayMode=0;
                if (app.shapes.size() > 1)
                    app.shapes.erase(app.shapes.begin() + 1, app.shapes.end());
                    
                glm::mat4 ellipsoidScale = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 1.0f, 0.5f));
                // ellipsoid
                app.shapes.emplace_back(
                    std::make_unique<Icosahedron>(
                        app.pMeshShader.get(), 
                        "var/icosahedron.txt", 
                        ellipsoidScale
                    )
                );
                break;
            }
            case GLFW_KEY_4:
            {
                app.mode = 4;    
                if (app.shapes.size() > 1)
                    app.shapes.erase(app.shapes.begin() + 1, app.shapes.end());
                glm::vec3 kColor = glm::vec3(1.0f, 0.5f, 0.31f);
                
                app.shapes.emplace_back(std::make_unique<Sphere>(
                app.pSphereShader.get(), glm::vec3(-3.0f, 0.0f, 0.0f), 1.0f, kColor, glm::mat4(1.0f)));


                // cylinder in middle
                app.shapes.emplace_back(std::make_unique<Cylinder>(
                    app.pCylinderShader.get(), glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 2.0f, kColor, glm::mat4(1.0f)));

                // cone on right
                app.shapes.emplace_back(std::make_unique<Cone>(
                    app.pConeShader.get(), glm::vec3(3.0f, 0.0f, 0.0f), 1.0f, 2.0f, kColor, glm::mat4(1.0f)));
                break;
            }
        
            case GLFW_KEY_5:
            {   
                app.mode = 5;    
                if (app.shapes.size() > 1)
                    app.shapes.erase(app.shapes.begin() + 1, app.shapes.end());
                glm::vec3 kColor = glm::vec3(0.8f, 0.2f, 0.8f);
        
                app.shapes.emplace_back(std::make_unique<Torus>(
                app.pTorusShader.get(), glm::vec3(0.0f, 0.0f, 0.0f), 2.0f, 0.6f, kColor, glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f))));
                break;
            }

           case GLFW_KEY_6:
            {
                app.mode = 6;
                if (app.shapes.size() > 1)
                    app.shapes.erase(app.shapes.begin() + 1, app.shapes.end());
                glm::vec3 kColor = glm::vec3(0.8f, 0.2f, 0.8f); 
                
                float e1 = 1.0f, e2 = 1.0f; 
                std::ifstream configFile("etc/config.txt");
                if (configFile.is_open()) {
                    configFile >> e1 >> e2;
                    configFile.close();
                    std::cout << "Loaded Super-quadric params: " << e1 << ", " << e2 << std::endl;
                } else {
                    std::cerr << "Warning: Could not open config.txt" << std::endl;
                }

                // add the SuperQuadric (Put it on the left)
                app.shapes.emplace_back(std::make_unique<SuperQuadric>(
                    app.pSuperQuadricShader.get(), glm::vec3(-2.0f, 0.0f, 0.0f), 1.5f, e1, e2, kColor, glm::mat4(1.0f)));
                    
                // Dodecahedron on the right
                app.shapes.emplace_back(std::make_unique<Dodecahedron>(
                app.pDodecahedronShader.get(), 
                glm::vec3(2.0f, 0.0f, 0.0f), 
                1.0f, 
                kColor, 
                glm::mat4(1.0f)
                ));
                break;
            }

            case GLFW_KEY_7:
            {
                app.mode = 7;
                if (app.shapes.size() > 1)
                    app.shapes.erase(app.shapes.begin() + 1, app.shapes.end());
                glm::mat4 cityBase = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));
                cityBase = glm::scale(cityBase, glm::vec3(0.3f));

                    // City Material Colors
                glm::vec3 concrete = glm::vec3(0.5f, 0.5f, 0.5f);
                glm::vec3 glass    = glm::vec3(0.2f, 0.5f, 0.8f);
                glm::vec3 white    = glm::vec3(0.9f, 0.9f, 0.9f);
                glm::vec3 roofRed  = glm::vec3(0.7f, 0.2f, 0.2f);
                glm::vec3 gold     = glm::vec3(0.8f, 0.7f, 0.2f);

                // 1                
                glm::mat4 b1_base = glm::translate(cityBase, glm::vec3(-5.0f, -5.0f, 0.0f));
                glm::mat4 b1_scale = glm::scale(b1_base, glm::vec3(1.5f, 1.5f, 5.0f)); 
                app.shapes.emplace_back(std::make_unique<SuperQuadric>(
                    app.pSuperQuadricShader.get(), glm::vec3(0), 1.0f, 0.1f, 0.1f, glass, b1_scale));

                // 2
                glm::mat4 b2_base = glm::translate(cityBase, glm::vec3(5.0f, -6.0f, 0.0f));
                app.shapes.emplace_back(std::make_unique<Cylinder>(
                    app.pCylinderShader.get(), glm::vec3(0), 2.0f, 3.0f, concrete, b2_base));
                // stack along z
                glm::mat4 b2_roof = glm::translate(b2_base, glm::vec3(0.0f, 0.0f, 1.5f));
                app.shapes.emplace_back(std::make_unique<Sphere>(
                    app.pSphereShader.get(), glm::vec3(0), 2.0f, white, b2_roof));

                // 3
                glm::mat4 b3_base = glm::translate(cityBase, glm::vec3(0.0f, -8.0f, 0.0f));
                app.shapes.emplace_back(std::make_unique<Cylinder>(
                    app.pCylinderShader.get(), glm::vec3(0), 3.0f, 0.5f, concrete, b3_base));
                glm::mat4 b3_ring = glm::translate(b3_base, glm::vec3(0.0f, 0.0f, 0.5f));
                app.shapes.emplace_back(std::make_unique<Torus>(
                    app.pTorusShader.get(), glm::vec3(0), 3.0f, 1.0f, white, b3_ring));

                // 4
                glm::mat4 b4_base = glm::translate(cityBase, glm::vec3(-7.0f, 4.0f, 0.0f));
                app.shapes.emplace_back(std::make_unique<Cylinder>(
                    app.pCylinderShader.get(), glm::vec3(0), 1.0f, 1.0f, concrete, b4_base));
                glm::mat4 b4_art = glm::translate(b4_base, glm::vec3(0.0f, 0.0f, 1.5f));
                app.shapes.emplace_back(std::make_unique<Dodecahedron>(
                    app.pDodecahedronShader.get(), glm::vec3(0), 1.2f, gold, b4_art));

                // 5
                glm::mat4 b5_base = glm::translate(cityBase, glm::vec3(7.0f, 5.0f, 0.0f));
                app.shapes.emplace_back(std::make_unique<Cylinder>(
                    app.pCylinderShader.get(), glm::vec3(0), 0.5f, 6.0f, white, b5_base));
                glm::mat4 b5_spire = glm::translate(b5_base, glm::vec3(0.0f, 0.0f, 3.0f));
                app.shapes.emplace_back(std::make_unique<Cone>(
                    app.pConeShader.get(), glm::vec3(0), 0.5f, 2.0f, roofRed, b5_spire));

                // 6
                glm::mat4 b6_base = glm::translate(cityBase, glm::vec3(0.0f, 8.0f, 0.0f));
                glm::mat4 b6_scale = glm::scale(b6_base, glm::vec3(3.0f, 2.0f, 1.5f));
                app.shapes.emplace_back(std::make_unique<SuperQuadric>(
                    app.pSuperQuadricShader.get(), glm::vec3(0), 1.0f, 0.1f, 0.1f, white, b6_scale));
                glm::mat4 b6_dome = glm::translate(b6_base, glm::vec3(0.0f, 0.0f, 1.5f));
                app.shapes.emplace_back(std::make_unique<Sphere>(
                    app.pSphereShader.get(), glm::vec3(0), 1.5f, roofRed, b6_dome));

                // 7
                glm::mat4 b7_baseA = glm::translate(cityBase, glm::vec3(-9.0f, 0.0f, 0.0f));
                glm::mat4 b7_scaleA = glm::scale(b7_baseA, glm::vec3(1.0f, 1.0f, 4.0f));
                app.shapes.emplace_back(std::make_unique<SuperQuadric>(
                    app.pSuperQuadricShader.get(), glm::vec3(0), 1.0f, 0.1f, 0.1f, glass, b7_scaleA));
                
                glm::mat4 b7_baseB = glm::translate(cityBase, glm::vec3(-9.0f, -2.5f, 0.0f));
                glm::mat4 b7_scaleB = glm::scale(b7_baseB, glm::vec3(1.0f, 1.0f, 4.0f));
                app.shapes.emplace_back(std::make_unique<SuperQuadric>(
                    app.pSuperQuadricShader.get(), glm::vec3(0), 1.0f, 0.1f, 0.1f, glass, b7_scaleB));

                // 8
                glm::mat4 b8_base = glm::translate(cityBase, glm::vec3(9.0f, 0.0f, 0.0f));
                app.shapes.emplace_back(std::make_unique<Cylinder>(
                    app.pCylinderShader.get(), glm::vec3(0), 1.5f, 4.0f, concrete, b8_base));
                glm::mat4 b8_roof = glm::translate(b8_base, glm::vec3(0.0f, 0.0f, 2.0f));
                app.shapes.emplace_back(std::make_unique<Cone>(
                    app.pConeShader.get(), glm::vec3(0), 1.6f, 1.5f, roofRed, b8_roof));
            }
        }

        if (key == GLFW_KEY_EQUAL && mods == GLFW_MOD_SHIFT ){
            if (app.mode == 2 ||app.mode == 3 )
            {
                auto* ico = dynamic_cast<Icosahedron*>(app.shapes.back().get());        
                ico->subdivide();}
                if (app.mode == 5){
                    auto* torus = dynamic_cast<Torus*>(app.shapes.back().get());        
                    int currentRes = torus->getResolution();
                    
                    if (currentRes < 60) {
                        torus->setResolution(currentRes * 2);
                    }
            }
        }
        if (key == GLFW_KEY_H && app.mode == 7 && !app.isFlying) {
            app.isFlying = true;
            app.flightMode = 'H';
            app.flightAngle = 0.0f;
            app.flightStartPos = app.camera.position;
            app.flightStartFront = app.camera.front; // Check your Camera class to see if these are capitalized or lowercase!
            app.flightStartRight = app.camera.right;
            app.flightStartUp = app.camera.up;
        }

        // Start Vertical Loop
        if (key == GLFW_KEY_V && app.mode == 7 && !app.isFlying) {
            app.isFlying = true;
            app.flightMode = 'V';
            app.flightAngle = 0.0f;
            app.flightStartPos = app.camera.position;
            app.flightStartFront = app.camera.front; // Check your Camera class to see if these are capitalized or lowercase!
            app.flightStartRight = app.camera.right;
            app.flightStartUp = app.camera.up;
        }

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
        }
    }
}


void App::scrollCallback(GLFWwindow * window, double xoffset, double yoffset)
{
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
    app.camera.processMouseScroll(yoffset);
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
    // Camera control
    App & app = *reinterpret_cast<App *>(glfwGetWindowUserPointer(window));

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        app.camera.processKeyboard(Camera::kLeft, app.timeElapsedSinceLastFrame);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        app.camera.processKeyboard(Camera::kRight, app.timeElapsedSinceLastFrame);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        app.camera.processKeyboard(Camera::kBackWard, app.timeElapsedSinceLastFrame);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        app.camera.processKeyboard(Camera::kForward, app.timeElapsedSinceLastFrame);
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        app.camera.processKeyboard(Camera::kUp, app.timeElapsedSinceLastFrame);
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        app.camera.processKeyboard(Camera::kDown, app.timeElapsedSinceLastFrame);
    }
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
    glLineWidth(2.0f);
    glPointSize(1.0f);
    glEnable(GL_DEPTH_TEST);

    initializeShadersAndObjects();
}


void App::initializeShadersAndObjects()
{
    pLineShader = std::make_unique<Shader>("src/shader/line.vert.glsl",
                                           "src/shader/line.frag.glsl");

    pMeshShader = std::make_unique<Shader>("src/shader/mesh.vert.glsl",
                                           "src/shader/phong.frag.glsl");

    pSphereShader = std::make_unique<Shader>("src/shader/sphere.vert.glsl",
                                             "src/shader/sphere.tesc.glsl",
                                             "src/shader/sphere.tese.glsl",
                                             "src/shader/phong.frag.glsl");

    // For Cylinder
    pCylinderShader = std::make_unique<Shader>(
        "src/shader/sphere.vert.glsl", // Reuse sphere vertex shader (it just passes the point)
        "src/shader/sphere.tesc.glsl", // Reuse sphere control shader
        "src/shader/cylinder.tese.glsl", // USE YOUR NEW CYLINDER TESE
        "src/shader/phong.frag.glsl"   // Use your standard lighting fragment shader
    );

    // For Cone
    pConeShader = std::make_unique<Shader>(
        "src/shader/sphere.vert.glsl",
        "src/shader/sphere.tesc.glsl",
        "src/shader/cone.tese.glsl",     
        "src/shader/phong.frag.glsl"
    );

        // For Torus
    pTorusShader = std::make_unique<Shader>(
        "src/shader/sphere.vert.glsl",
        "src/shader/sphere.tesc.glsl",
        "src/shader/torus.tese.glsl",     
        "src/shader/phong.frag.glsl"
    );
    pDodecahedronShader = std::make_unique<Shader>(
        "src/shader/dodecahedron.vert.glsl", // We can reuse the basic vert shader!
        "src/shader/dodecahedron.tesc.glsl",
        "src/shader/dodecahedron.tese.glsl",
        "src/shader/phong.frag.glsl"
    );

    // For Super-quadric
    pSuperQuadricShader = std::make_unique<Shader>(
        "src/shader/sphere.vert.glsl",       // Reuse the pass-through vertex shader
        "src/shader/sphere.tesc.glsl",       // Reuse the control shader (handling uTessLevel)
        "src/shader/superquadric.tese.glsl", // Your new super-quadric math evaluation shader!
        "src/shader/phong.frag.glsl"         // Standard lighting
    );

    shapes.emplace_back(
            std::make_unique<Line>(
                    pLineShader.get(),
                    std::vector<Line::Vertex> {
                            {{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                            {{3.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
                            {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                            {{0.0f, 3.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                            {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
                            {{0.0f, 0.0f, 3.0f}, {0.0f, 0.0f, 1.0f}},
                    },
                    glm::mat4(1.0f)
            )
    );

        //   shapes.emplace_back(std::make_unique<Cylinder>(
        //         pCylinderShader.get(), glm::vec3(0.0f, 0.0f, 0.0f), 3.0f, 5.0f, glm::vec3(1,0,0), glm::mat4(1.0f)));


    // shapes.emplace_back(
    //         std::make_unique<Tetrahedron>(
    //                 pMeshShader.get(),
    //                 "var/tetrahedron.txt",
    //                 glm::translate(glm::mat4(1.0f), {-2.0f, 0.0f, 0.0f})
    //         )
    // );

    // // Add the Cube
    // shapes.emplace_back(
    //     std::make_unique<Cube>(
    //         pMeshShader.get(),
    //         "var/cube.txt",
    //         glm::translate(glm::mat4(1.0f), {0.0f, 0.0f, 0.0f}) // Center it
    //     )
    // );

    // shapes.emplace_back(
    //     std::make_unique<Octahedron>(
    //         pMeshShader.get(),
    //         "var/octahedron.txt",
    //         glm::translate(glm::mat4(1.0f), {2.0f, 0.0f, 0.0f}) // Move it to the right
    //     )
    // );

    // shapes.emplace_back(
    //         std::make_unique<Mesh>(
    //                 pMeshShader.get(),
    //                 std::vector<Mesh::Vertex> {
    //                         {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
    //                         {{0.5f,  -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}},
    //                         {{0.0f,  0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
    //                 },
    //                 glm::rotate(
    //                         glm::translate(glm::mat4(1.0f), {2.0f, 0.0f, 0.0f}),
    //                         glm::radians(45.0f), {0.0f, 1.0f, 0.0f}
    //                 )
    //         )
    // );

    // shapes.emplace_back(
    //         std::make_unique<Sphere>(
    //                 pSphereShader.get(),
    //                 glm::vec3(0.0f, 0.0f, 0.0f),
    //                 1.0f,
    //                 glm::vec3(1.0f, 0.5f, 0.31f),
    //                 glm::mat4(1.0f)
    //         )
    // );
}


void App::render()
{
    auto t = static_cast<float>(timeElapsedSinceLastFrame);

    // Update shader uniforms
    view = camera.getViewMatrix();
    projection = glm::perspective(glm::radians(camera.zoom),
                                  static_cast<GLfloat>(kWindowWidth) / static_cast<GLfloat>(kWindowHeight),
                                  0.01f,
                                  100.0f);

    // normal camera position for the lighting shader
    glm::vec3 currentCameraPos = camera.position; 

    
    // if the user pressed H or V, modify view
    if (isFlying){
        float flightSpeed = 1.0f;
        flightAngle += t * flightSpeed;

        if (flightAngle >= glm::radians(360.0f)) {
            isFlying = false; // back to normal
        } else {
            glm::vec3 planePos, planeForward, planeUp;
            float radius = 15.0f; 
            
            float c = (float)cos(flightAngle);
            float s = (float)sin(flightAngle);

            if (flightMode == 'H') {
                // HORIZONTAL ORBIT
                glm::vec3 loopCenter = flightStartPos + (flightStartRight * radius);
                
                planePos = loopCenter 
                           - (flightStartRight * (radius * c)) 
                           + (flightStartFront * (radius * s));

                planeForward = (flightStartRight * s) 
                               + (flightStartFront * c);
                
                planeUp = flightStartUp; 
                planeForward -= flightStartUp * 0.3f; 
            } 
            else if (flightMode == 'V') {
                // VERTICAL LOOP
                glm::vec3 loopCenter = flightStartPos + (flightStartUp * radius);

                planePos = loopCenter 
                           - (flightStartUp * (radius * c)) 
                           + (flightStartFront * (radius * s));

                planeForward = (flightStartUp * s) 
                               + (flightStartFront * c);
                
                planeUp = (flightStartUp * c) 
                          - (flightStartFront * s);
            }

            // new view matrix
            view = glm::lookAt(planePos, planePos + planeForward, glm::normalize(planeUp));
            currentCameraPos = planePos; 
        }
    }
    

    pLineShader->use();
    pLineShader->setMat4("view", view);
    pLineShader->setMat4("projection", projection);

    if (mode == 7 && isFlying){
          pMeshShader->setVec3("ViewPos", currentCameraPos);
    pSphereShader->setVec3("ViewPos", currentCameraPos);
    pCylinderShader->setVec3("ViewPos", currentCameraPos);
    pConeShader->setVec3("ViewPos", currentCameraPos);
    pTorusShader->setVec3("ViewPos", currentCameraPos);
    pDodecahedronShader->setVec3("ViewPos", currentCameraPos);
    pSuperQuadricShader->setVec3("ViewPos",currentCameraPos);
    }
    else{
    pMeshShader->setVec3("ViewPos", camera.position);
    pSphereShader->setVec3("ViewPos", camera.position);
    pCylinderShader->setVec3("ViewPos", camera.position);
    pConeShader->setVec3("ViewPos", camera.position);
    pTorusShader->setVec3("ViewPos", camera.position);
    pDodecahedronShader->setVec3("ViewPos", camera.position);
    pSuperQuadricShader->setVec3("ViewPos", camera.position);
}

    pMeshShader->use();
    pMeshShader->setMat4("view", view);
    pMeshShader->setMat4("projection", projection);
    pMeshShader->setVec3("lightPos", lightPos);
    pMeshShader->setVec3("lightColor", lightColor);

    pSphereShader->use();
    pSphereShader->setMat4("view", view);
    pSphereShader->setMat4("projection", projection);
    pSphereShader->setVec3("lightPos", lightPos);
    pSphereShader->setVec3("lightColor", lightColor);

    pMeshShader->use();
    pMeshShader->setInt("uMode", displayMode);

    pSphereShader->use();
    pSphereShader->setInt("uMode", displayMode);

    pCylinderShader->use();
    pCylinderShader->setMat4("view", view);
    pCylinderShader->setMat4("projection", projection);
    pCylinderShader->setVec3("lightPos", lightPos);
    pCylinderShader->setVec3("lightColor", lightColor);
    pCylinderShader->setInt("uMode", displayMode);

    pConeShader->use();
    pConeShader->setMat4("view", view);
    pConeShader->setMat4("projection", projection);
    pConeShader->setVec3("lightPos", lightPos);
    pConeShader->setVec3("lightColor", lightColor);
    pConeShader->setInt("uMode", displayMode);
    
    pTorusShader->use();
    pTorusShader->setMat4("view", view);
    pTorusShader->setMat4("projection", projection);
    pTorusShader->setVec3("lightPos", lightPos);
    pTorusShader->setVec3("lightColor", lightColor);
    pTorusShader->setInt("uMode", displayMode);

    
    pDodecahedronShader->use();
    pDodecahedronShader->setMat4("view", view);
    pDodecahedronShader->setMat4("projection", projection);
    pDodecahedronShader->setVec3("lightPos", lightPos);
    pDodecahedronShader->setVec3("lightColor", lightColor);
    pDodecahedronShader->setInt("uMode", displayMode);

        
    pSuperQuadricShader->use();
    pSuperQuadricShader->setMat4("view", view);
    pSuperQuadricShader->setMat4("projection", projection);
    pSuperQuadricShader->setVec3("lightPos", lightPos);
    pSuperQuadricShader->setVec3("lightColor", lightColor);
    pSuperQuadricShader->setInt("uMode", displayMode);


    // Render.
    // for (auto & s : shapes)
    // {
    //     if (s->visible)
    //         s->render(t);
    // }
    // glDisable(GL_CULL_FACE);
    for (size_t i = 0; i < shapes.size(); ++i) {
        if (i == 0 && !axes) {
            continue;
        }
        shapes[i]->render(timeElapsedSinceLastFrame);
    }
}
