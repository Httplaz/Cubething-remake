#pragma once
#include <string>
#include <lodepng.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Texture.h"

using namespace std;
using namespace glm;
class Skybox
{
public:
	Skybox(string path);
	Skybox();
	void render(mat4 view, mat4 projection);
private:
	uint32_t loadCubemap(vector<string> facePaths);
    uint32_t VAO, VBO;
    uint32_t cubemap;
    Shader shader;
    float skyboxVertices[108] = 
    {
        // positions          
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         -1.0f,  1.0f, -1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,

         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         -1.0f, -1.0f,  1.0f,

        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,

         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         -1.0f,  1.0f, -1.0f,

        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         -1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
    };

    vector<string> faces = 
    {
            "right.png",
            "left.png",
            "top.png",
            "bottom.png",
            "front.png",
            "back.png"
    };
};

