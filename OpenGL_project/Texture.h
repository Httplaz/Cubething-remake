#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include <glad/glad.h>

class Texture
{
private:
    uint32_t texture;
    int width, height;
public:
    Texture();
    static void saveImage(const char* name, uint8_t* data, int width, int height, int channels);
    Texture(const GLchar* path, bool linear);
    static uint8_t* loadImage(std::string path);
    GLuint getTexture();
};