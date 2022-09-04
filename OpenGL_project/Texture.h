#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "lodepng.h"

#include <glad/glad.h>

class Texture
{
private:
    uint32_t texture;
    uint32_t width, height;
public:
    Texture();
    Texture(const GLchar* path, bool linear);
    GLuint getTexture();
};