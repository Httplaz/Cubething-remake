
#include <iostream>
#define GLFW_INCLUDE_NONE

// GLFW
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <thread>
#include "Shader.h"
#include "Screen.h"
#include "Game.h"


GLFWwindow* window;

unsigned int VAO, VBO;

Shader screenShader;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


double frameRenderDuration = 1. / 120;
double frameUpdateDuration = 1. / 120;
dvec2 cursorPos;
bool firstMouse = true;
bool fullScreen = false;
ivec2 screenResulution;
ivec2 windowPosBackup;
ivec2 windowSizeBackup;

Game* game;

int tp = 0; \

double latestFrameRenderTime;
double latestFrameUpdateTime;

glm::ivec2 getResolution()
{
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    return glm::ivec2(mode->width, mode->height);
}

void update()
{
    int bias = 10;
    while (!glfwWindowShouldClose(window))
    {
        double currentUpdateTime = glfwGetTime();
        double frameUpdateTime = currentUpdateTime - latestFrameUpdateTime;
        if (frameUpdateTime >= frameUpdateDuration)
        {
            dvec2 a;
            glfwGetCursorPos(window, &(a.x), &a.y);
            if (!firstMouse)
                game->setRotationInput((dvec2(a.x - cursorPos.x, cursorPos.y - a.y)) / 10.);
            cursorPos = a;
            firstMouse = false;
            game->update();
            latestFrameUpdateTime = currentUpdateTime;
            //string displayString = "FPS: " + to_string(1. / frameUpdateTime);
            //glfwSetWindowTitle(window, displayString.c_str());
        }
        else
            this_thread::sleep_for(chrono::milliseconds(int((frameUpdateDuration - frameUpdateTime-bias) * 1000.)));
    }
}

void render()
{
    int bias = 10;
    while (!glfwWindowShouldClose(window))
    {
        double currentRenderTime = glfwGetTime();
        double frameRenderTime = currentRenderTime - latestFrameRenderTime;
        if (frameRenderTime >= frameRenderDuration) 
        {

            game->render();
            glfwPollEvents();
            glfwSwapBuffers(window);
            latestFrameRenderTime = currentRenderTime;
            tp++;
            string displayString = "FPS: " + to_string(1. / frameRenderTime);
            glfwSetWindowTitle(window, displayString.c_str());
           

            dvec2 a;
            glfwGetCursorPos(window, &(a.x), &a.y);
            if (!firstMouse)
                game->setRotationInput((dvec2(a.x - cursorPos.x, cursorPos.y - a.y)) / 10.);
            cursorPos = a;
            firstMouse = false;
            game->update();



           if (frameRenderTime < frameRenderDuration)
                this_thread::sleep_for(chrono::milliseconds(int((frameRenderDuration-frameRenderTime-bias)*1000.)));
        }
    }
    
}

int main()
{ 
    cout << "START\n";
    glfwInit();
    cout << "gldwfsf";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    std::vector<float> vertices;

    int width, height;
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    width = mode->width;
    height = mode->height;
    screenResulution = ivec2(width, height);
    cout << "creating window\n";
    window = glfwCreateWindow(width / 2, height / 2, "RicardoGameTezt", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    cout << "game constructor\n";
    game = new Game();
    cout << "game ready\n";
    game->updateAspectRatio(float(width) / float(height));
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwGetCursorPos(window, &(cursorPos.y), &cursorPos.x);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetKeyCallback(window, key_callback);
    //glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    std::cout << "Hello World!\n";
    //thread thr(update);
    //thr.detach();
    render();

    glfwTerminate();
    return 0;
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    game->updateAspectRatio(float(width) / float(height));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    //std::cout << origin1.z << "\n\n\n\n\n";
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_W)
            game->addMovementInput(vec3(0, 0, 1));
        if (key == GLFW_KEY_A)
            game->addMovementInput(vec3(-1, 0, 0));
        if (key == GLFW_KEY_S)
            game->addMovementInput(vec3(0, 0, -1));
        if (key == GLFW_KEY_D)
            game->addMovementInput(vec3(1, 0, 0));
        if (key == GLFW_KEY_Q)
            game->addMovementInput(vec3(0, -1, 0));
        if (key == GLFW_KEY_E)
            game->addMovementInput(vec3(0, 1, 0));
        if (key == GLFW_KEY_RIGHT)
            game->addRotationInput(vec2(1, 0));
        if (key == GLFW_KEY_LEFT)
            game->addRotationInput(vec2(-1, 0));
        if (key == GLFW_KEY_UP)
            game->addRotationInput(vec2(0, 1));
        if (key == GLFW_KEY_DOWN)
            game->addRotationInput(vec2(0, -1));
    }

    if (action == GLFW_RELEASE)
    {
        if (key == GLFW_KEY_W)
            game->addMovementInput(vec3(0, 0, -1));
        if (key == GLFW_KEY_A)
            game->addMovementInput(vec3(1, 0, 0));
        if (key == GLFW_KEY_S)
            game->addMovementInput(vec3(0, 0, 1));
        if (key == GLFW_KEY_D)
            game->addMovementInput(vec3(-1, 0, 0));
        if (key == GLFW_KEY_Q)
            game->addMovementInput(vec3(0, 1, 0));
        if (key == GLFW_KEY_E)
            game->addMovementInput(vec3(0, -1, 0));
        if (key == GLFW_KEY_RIGHT)
            game->addRotationInput(vec2(-1, 0));
        if (key == GLFW_KEY_LEFT)
            game->addRotationInput(vec2(1, 0));
        if (key == GLFW_KEY_UP)
            game->addRotationInput(vec2(0, -1));
        if (key == GLFW_KEY_DOWN)
            game->addRotationInput(vec2(0, 1));
        //if (key == GLFW_KEY_X)
            //game->chunkholder->chunkRing.shift({ 1,0,0 });

        if (key == GLFW_KEY_F)
        {
            fullScreen = !fullScreen;
            if (fullScreen) 
            {
                glfwGetWindowSize(window, &windowSizeBackup.x, &windowSizeBackup.y);
                glfwGetWindowPos(window, &windowPosBackup.x, &windowPosBackup.y);
                glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, screenResulution.x, screenResulution.y, 120);
            }
            else
                glfwSetWindowMonitor(window, nullptr, windowPosBackup.x, windowPosBackup.y, windowSizeBackup.x, windowSizeBackup.y, 120);
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        //cursorPos.x = xpos;
        //cursorPos.y = ypos;
        //firstMouse = false;
    }

    //float xoffset = xpos - cursorPos.x;
    //float yoffset = cursorPos.y - ypos; // reversed since y-coordinates go from bottom to top

    //cursorPos.x = xpos;
    //cursorPos.y = ypos;
    //game->setRotationInput(vec2(xoffset, yoffset));
}