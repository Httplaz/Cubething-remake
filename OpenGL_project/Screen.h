#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "Shader.h"
#include "Texture.h"
#include "Vertexpool.h"
#include "Camera.h"
#include <lodepng.h>

class Screen
{
public:
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static float vertices[];
	static int width;
	static int height;
	static unsigned int VBO, VAO, indbo;
	static Shader screenShader;
public:
	static void createWindow(int width, int height);
	static void renderScreenMesh();
	static GLFWwindow* getWindow();
	static GLFWwindow* window;
};

