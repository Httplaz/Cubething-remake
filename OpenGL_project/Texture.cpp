#include "Texture.h"

Texture::Texture()
{
}

Texture::Texture(const GLchar* path, bool linear)
{
    std::vector<unsigned char> image;
    unsigned error = lodepng::decode(image, width, height, path);
    std::cout << width << " " << height << "\n";


    if (error != 0)
    {
        std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GL_LINEAR_MIPMAP_LINEAR));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (linear ? GL_LINEAR : GL_NEAREST));
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image.front());
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture::getTexture()
{
    return texture;
}