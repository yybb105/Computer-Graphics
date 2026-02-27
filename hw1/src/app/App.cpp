#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "app/App.h"
#include "shape/Pixel.h"
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
        glClear(GL_COLOR_BUFFER_BIT);

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
        // // Note: Must calculate offset first, then update lastMouseLeftPressPos.
        // glm::dvec2 offset = app.mousePos - app.lastMouseLeftPressPos;
        app.lastMouseLeftPressPos = app.mousePos;
    }

    // Display a preview line which moves with the mouse cursor iff.
    // the most-recent mouse click is left click.
    // showPreview is controlled by mouseButtonCallback.
    if (app.showPreview && app.mode > 0)
    {
        // use the most recently appended shape object
        auto nptr = dynamic_cast<Pixel *>(app.shapes.back().get());
        
        // set default start & end points to last press and current cursor pos
        auto x0 = static_cast<int>(app.lastMouseLeftPressPos.x);
        auto y0 = static_cast<int>(app.lastMouseLeftPressPos.y);
        auto x1 = static_cast<int>(app.mousePos.x);
        auto y1 = static_cast<int>(app.mousePos.y);
        
        nptr->path.clear();
    
        if (app.mode == 4){ // use circle or ellipse drawing functions if user pressed 4
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
                bresenhamCircle(nptr->path,x0,y0,x1,y1);
            else
                bresenhamEllipse(nptr->path, x0, y0, x1, y1);
        }
        else if (app.mode < 5) // use line if user pressed 1 or 3
            bresenhamLine(nptr->path, x0, y0, x1, y1);
        nptr->dirty = true; // notify gl of change
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

    // switch modes according to key press

    if (key == GLFW_KEY_1 && action == GLFW_RELEASE)
    {
        app.showPreview = false;
        // clear window and append a new object for single-line drawing
        app.shapes.erase(app.shapes.begin() + 1, app.shapes.end());
        auto nline = std::make_unique<Pixel>(app.pPixelShader.get());
        app.shapes.push_back(std::move(nline));
        app.mode = 1;
    }
    if (key == GLFW_KEY_3 && action == GLFW_RELEASE)
    {
        app.showPreview = false;
        // clear window and wait for click to add new line
        app.shapes.erase(app.shapes.begin() + 1, app.shapes.end());
        app.mode = 3;
    }
    if (key == GLFW_KEY_4 && action == GLFW_RELEASE)
    {
        app.showPreview = false;
        // clear window and append a new object for single circle drawing
        app.shapes.erase(app.shapes.begin() + 1, app.shapes.end());
        auto nline = std::make_unique<Pixel>(app.pPixelShader.get());
        app.shapes.push_back(std::move(nline));
        app.mode = 4;
    }
    if (key == GLFW_KEY_5 && action == GLFW_RELEASE)
    {
        app.showPreview = false;
        app.shapes.erase(app.shapes.begin() + 1, app.shapes.end()); // clear window        
        app.mode = 5;
        std::ifstream file("etc/config.txt");   // read file
        if (!file.is_open())         
            std::cout << "Failed to open config file." << std::endl;
        int type;
        file >> type;   // read type

        if (type == 2){
            auto nline = std::make_unique<Pixel>(app.pPixelShader.get());
            app.shapes.push_back(std::move(nline));
            auto nptr = dynamic_cast<Pixel *>(app.shapes.back().get());
            float a2,a1,a0;
            file >> a2 >> a1 >> a0;
            bresenhamQuadratic(nptr->path,a2,a1,a0);
            nptr->dirty=true;
        }
        else if (type == 1){
            auto nline = std::make_unique<Pixel>(app.pPixelShader.get());
            app.shapes.push_back(std::move(nline));
            auto nptr = dynamic_cast<Pixel *>(app.shapes.back().get());
            float a3,a2,a1,a0;
            file >> a3 >> a2 >> a1 >> a0;
            bresenhamCubic(nptr->path,a3,a2,a1,a0);
            nptr->dirty=true;
        }

    }
    if (key == GLFW_KEY_C && action == GLFW_PRESS){
        app.enclose = true;
    }
    if (key == GLFW_KEY_C && action == GLFW_RELEASE){
        app.enclose = false;
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
            if (app.mode > 0 & app.mode < 5)
                app.showPreview = true;

            if (app.mode == 3){
                if (app.polydone)    // re-start if a poly line or polygon was finished
                    app.shapes.erase(app.shapes.begin() + 1, app.shapes.end());
                app.polydone = false;
                
                auto nline = std::make_unique<Pixel>(app.pPixelShader.get());   // append shape
                app.shapes.push_back(std::move(nline));
            }
      
            #ifdef DEBUG_MOUSE_POS
            std::cout << "[ " << app.mousePos.x << ' ' << app.mousePos.y << " ]\n";
            #endif
        }
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_RELEASE)
        {   
            if (app.enclose && app.mode == 3) {        // if in poly line mode, check if C is being held
                auto nline = std::make_unique<Pixel>(app.pPixelShader.get());   // add the closing edge
                app.shapes.push_back(std::move(nline));

                auto nptr = dynamic_cast<Pixel *>(app.shapes.back().get());

                auto fromline = dynamic_cast<Pixel *>(app.shapes[app.shapes.size()-2].get());   // the previous line
                auto toline = dynamic_cast<Pixel *>(app.shapes[1].get());   // the first line

                Pixel::Vertex& v1 = fromline->path.back();  // end of prev line 
                Pixel::Vertex& v2 = toline->path.front();  // start first line

                auto x0 = static_cast<int>(v1.position.x);
                auto y0 = static_cast<int>(v1.position.y);
                auto x1 = static_cast<int>(v2.position.x);
                auto y1 = static_cast<int>(v2.position.y);
                
                nptr->path.clear();
                bresenhamLine(nptr->path, x0, y0, x1, y1);  // draw line
                nptr->dirty = true;
                app.enclose = false;
            }
            app.polydone = true;
            app.showPreview = false;
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


void App::bresenhamLine(std::vector<Pixel::Vertex> & path, int x0, int y0, int x1, int y1)
{
    int dx = std::abs(x1 - x0); // abs difference in x
    int dy = std::abs(y1 - y0); // difference in y
    int p = 2 * dy - dx;
    int p2 = 2 * dx - dy;
    int twoDy = 2 * dy;
    int twoDyMinusDx = 2 * (dy - dx);

    int x = x0;
    int y = y0;

    path.emplace_back(x, y, 1.0f, 1.0f, 1.0f);

    if (dx == 0){   // vertical
        while (y < y1){
            ++y;
            path.emplace_back(x, y, 1.0f, 1.0f, 1.0f);
        }
        while (y > y1){
            --y;
            path.emplace_back(x, y, 1.0f, 1.0f, 1.0f);
        }    
    }

    while (dy<dx && x != x1)
    {
        x = (x < x1)?x+1:(x > x1)? x-1:x;

        if (p < 0)
        {
            p += twoDy;
        }
        else
        {   
            y = (y < y1)?y+1:(y > y1)? y-1:y;
            p += twoDyMinusDx;
        }

        path.emplace_back(x, y, 1.0f, 1.0f, 1.0f);
    }

    while (dy>dx && y != y1)
    {
        y = (y < y1)?y+1:(y > y1)? y-1:y;

        if (p2 < 0)
        {
            p2 += 2*dx;
        }
        else
        {   
            x = (x < x1)?x+1:(x > x1)? x-1:x;
            p2 += (2*dx - 2*dy);
        }

        path.emplace_back(x, y, 1.0f, 1.0f, 1.0f);
    }
 
}

void App::bresenhamCircle(std::vector<Pixel::Vertex> & path, int cx, int cy, int px, int py){
    int dx = px - cx;
    int dy = py - cy;
    int r = static_cast<int>(std::sqrt(dx*dx + dy*dy));

    int x = 0, y = r;
    int d = 3 - 2 * r;
    
    while (x <= y) {
        // 8 points
        path.emplace_back(cx + x, cy + y, 1.0f, 1.0f, 1.0f);
        path.emplace_back(cx - x, cy + y, 1.0f, 1.0f, 1.0f);
        path.emplace_back(cx + x, cy - y, 1.0f, 1.0f, 1.0f);
        path.emplace_back(cx - x, cy - y, 1.0f, 1.0f, 1.0f);
        path.emplace_back(cx + y, cy + x, 1.0f, 1.0f, 1.0f);
        path.emplace_back(cx - y, cy + x, 1.0f, 1.0f, 1.0f);
        path.emplace_back(cx + y, cy - x, 1.0f, 1.0f, 1.0f);
        path.emplace_back(cx - y, cy - x, 1.0f, 1.0f, 1.0f);
        
        if (d < 0) {
            d += 4 * x + 6;
        } else {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void App::bresenhamEllipse(std::vector<Pixel::Vertex>& path, int x0, int y0, int x1, int y1) {
    int a = std::abs(x1-x0), b = std::abs(y1-y0);
    int x = 0, y = b;
    int a2 = a * a, b2 = b * b;
    int two_a2 = 2 * a2, two_b2 = 2 * b2;
    
    // 1st region
    int d1 = b2 - a2 * b + a2 / 4;
    while (a2 * y > b2 * x) {
        path.emplace_back(x0 + x, y0 + y, 1.0f, 1.0f, 1.0f);
        path.emplace_back(x0 - x, y0 + y, 1.0f, 1.0f, 1.0f);
        path.emplace_back(x0 + x, y0 - y, 1.0f, 1.0f, 1.0f);
        path.emplace_back(x0 - x, y0 - y, 1.0f, 1.0f, 1.0f);
        
        if (d1 < 0) {
            d1 += two_b2 * (2 * x + 3);
        } else {
            d1 += two_b2 * (2 * x + 3) - two_a2 * (2 * y - 2);
            y--;
        }
        x++;
    }
    
    // 2nd
    int d2 = b2 * (x + 0.5) * (x + 0.5) + a2 * (y - 1) * (y - 1) - a2 * b2;
    while (y >= 0) {
        path.emplace_back(x0 + x, y0 + y, 1.0f, 1.0f, 1.0f);
        path.emplace_back(x0 - x, y0 + y, 1.0f, 1.0f, 1.0f);
        path.emplace_back(x0 + x, y0 - y, 1.0f, 1.0f, 1.0f);
        path.emplace_back(x0 - x, y0 - y, 1.0f, 1.0f, 1.0f);
        
        if (d2 > 0) {
            d2 += two_a2 * (3 - 2 * y);
        } else {
            d2 += two_b2 * (2 * x + 2) + two_a2 * (3 - 2 * y);
            x++;
        }
        y--;
    }
}

void App::bresenhamQuadratic(std::vector<Pixel::Vertex>& path, float a2, float a1, float a0) {
    for (int mathX = -kWindowWidth/2; mathX <= kWindowWidth/2; mathX++) {
        float mathY = a2*mathX*mathX + a1*mathX + a0;
        
        // need to make center the origin
        int screenX = kWindowWidth/2 + mathX;
        int screenY = kWindowHeight/2 + static_cast<int>(mathY); 
        
        if (screenX >= 0 && screenX < kWindowWidth &&
            screenY >= 0 && screenY < kWindowHeight) {
            path.emplace_back(screenX, screenY, 1.0f, 1.0f, 1.0f);
        }
    }
}

void App::bresenhamCubic(std::vector<Pixel::Vertex>& path,
                   float a3, float a2, float a1, float a0) {
    int prevX = -kWindowWidth/2;
    float prevY = a3*prevX*prevX*prevX + a2*prevX*prevX + a1*prevX + a0;
    
    for (int mathX = -kWindowWidth/2 + 1; mathX <= kWindowWidth/2; mathX++) {
        float mathY = a3*mathX*mathX*mathX + a2*mathX*mathX + a1*mathX + a0;
        
        float derivative = 3*a3*mathX*mathX + 2*a2*mathX + a1;
        
        int screenX = kWindowWidth/2 + mathX;
        int screenY = kWindowHeight/2 + static_cast<int>(mathY);
        
        path.emplace_back(screenX, screenY, 1.0f, 1.0f, 1.0f);
        
        prevX = mathX;
        prevY = mathY;
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
    glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    glLineWidth(1.0f);
    glPointSize(1.0f);

    // Initialize shaders and objects-to-render;
    pPixelShader = std::make_unique<Shader>("src/shader/pixel.vert.glsl",
                                            "src/shader/pixel.frag.glsl");

    shapes.emplace_back(std::make_unique<Pixel>(pPixelShader.get()));
}


void App::render()
{
    // Update all shader uniforms.
    pPixelShader->use();
    pPixelShader->setFloat("windowWidth", kWindowWidth);
    pPixelShader->setFloat("windowHeight", kWindowHeight);

    // Render all shapes.
    for (auto & s : shapes)
    {
        s->render();
    }
}