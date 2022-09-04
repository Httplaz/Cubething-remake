#include "Skybox.h"

Skybox::Skybox(string path)
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glCullFace(GL_TRUE);
    glDepthMask(GL_TRUE);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    vector<string> facePath = faces;
    for (int i = 0; i < 6; i++)
        facePath[i] = path + "/" + facePath[i];
    cubemap = loadCubemap(facePath);
    shader = Shader("skyboxShaderVert.glsl", "skyboxShaderFrag.glsl");
}

Skybox::Skybox()
{
}

void Skybox::render(mat4 view, mat4 projection)
{
    glDepthMask(GL_FALSE);
    shader.use();
    shader.setMat4(mat4(mat3(view)), "view");
    shader.setMat4(projection, "projection");
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
}


uint32_t Skybox::loadCubemap(vector<string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    uint32_t width, height;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        std::vector<unsigned char> image;
        lodepng::decode(image, width, height, faces[i]);
        if (!image.empty())
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data()
            );
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    return textureID;
}