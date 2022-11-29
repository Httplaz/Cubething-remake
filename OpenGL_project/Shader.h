#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <glad/glad.h>

class Shader
{
private:
    //GLuint program;
public:
    GLuint program;
    //glBufferStorage
    Shader();
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath);
    void use();
    GLuint getUniformLocation(const GLchar* name);
    void setMat4(glm::mat4 m, std::string name);
    void setVec3(glm::vec3 v, std::string name);
    void setIvec3(glm::ivec3 v, std::string name);
    void setIvec4(glm::ivec4 v, std::string name);
    void setInt(int i, std::string name);
    void setFloat(float i, std::string name);
    void setFloatArray(std::vector<float> arr, std::string name);
    void setIntArray(std::vector<int>& arr, std::string name);
};