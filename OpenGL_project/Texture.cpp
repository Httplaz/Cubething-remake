#include "Texture.h"

#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

Texture::Texture()
{
}

void Texture::saveImage(const char* name, uint8_t* data, int width, int height, int channels)
{
    stbi_write_png(name, width, height, 4, data, 0);
}

Texture::Texture(const GLchar* path, bool linear)
{
    unsigned char* imgData;


    //std::vector<unsigned char> image;
    //unsigned error = lodepng::decode(image, width, height, path);
    //std::cout << width << " " << height << "\n";

    bool png = true;

    //if (error != 0)
    //{
        //png = false;
        //std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
        int nrChannels;
        imgData = stbi_load(path, &width, &height, &nrChannels, 0);
        //width = uint32_t(width0);
        //height = uint32_t(height0);
    
    //else
        //imgData = image.data();

    uint32_t glFormat = 0;
    switch (nrChannels)
    {
    case 1:
        glFormat = GL_RED;
        break;
    case 2:
        glFormat = GL_RG;
        break;
    case 3:
        glFormat = GL_RGB;
        break;
    case 4:
        glFormat = GL_RGBA;
        break;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GL_LINEAR_MIPMAP_LINEAR));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (linear ? GL_LINEAR : GL_NEAREST));
    glTexImage2D(GL_TEXTURE_2D, 0, glFormat, width, height, 0, glFormat, GL_UNSIGNED_BYTE, imgData);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(imgData);
}

uint8_t* Texture::loadImage(std::string path, uint32_t channels)
{
    unsigned char* imgData;
    int width, height, nrChannels;
    imgData = stbi_load(path.c_str(), &width, &height, &nrChannels, channels);
    if(!imgData)
        imgData = stbi_load((path+".png").c_str(), &width, &height, &nrChannels, channels);
    if (!imgData)
        imgData = stbi_load((path + ".jpg").c_str(), &width, &height, &nrChannels, channels);
    if (!imgData) 
    {
        std::cout << "image load failed\n";
        return nullptr;
    }
    std::cout << path << " " << width << " " << height << " " << nrChannels << "\n";
    uint8_t* data = new uint8_t[width * height * nrChannels];
    //std::cout << path << " " << nrChannels << "\n";
    memcpy(data, imgData, width * height * channels * sizeof(uint8_t));
    stbi_image_free(imgData);
    return data;
}

GLuint Texture::getTexture()
{
    return texture;
}