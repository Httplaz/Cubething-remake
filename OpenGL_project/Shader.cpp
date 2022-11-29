
// This is an independent project of an individual developer. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "Shader.h"


Shader::Shader()
{}

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath) : Shader(vertexPath, NULL, fragmentPath)
{

}

Shader::Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath)
{
    if (geometryPath)
        std::cout << "NEGRO";
    std::string vertexCode;
    std::string geometryCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream gShaderFile;
    std::ifstream fShaderFile;
   
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        if(geometryPath)
            gShaderFile.open(geometryPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, gShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        if (geometryPath)
            gShaderStream << gShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        if (geometryPath)
            gShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        if (geometryPath)
            geometryCode = gShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* gShaderCode = geometryCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertex, geometry, fragment;
    GLint success;
    GLchar infoLog[4096];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 4096, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 4096, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << "eblan\n" << infoLog << std::endl;
    };

    if (geometryPath)
    {
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(geometry, 4096, NULL, infoLog);
            std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << "eblan\n" << infoLog << std::endl;
        };
    }

    program = glCreateProgram();
    glAttachShader(program, vertex);
    if(geometryPath)
        glAttachShader(program, geometry);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(this->program, 4096, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    if(geometryPath)
        glDeleteShader(geometry);
    glDeleteShader(fragment);
}

void Shader::use()
{
    glUseProgram(program);
}

GLuint Shader::getUniformLocation(const GLchar* name)
{
    return glGetUniformLocation(program, name);
}

void Shader::setMat4(glm::mat4 m, std::string name)
{
    int loc = getUniformLocation(name.c_str());
    glProgramUniformMatrix4fv(program, loc, 1, GL_FALSE, glm::value_ptr(m));
}

void Shader::setVec3(glm::vec3 v, std::string name)
{
    int loc = getUniformLocation(name.c_str());
    glProgramUniform3f(program, loc, v.x, v.y, v.z);
}

void Shader::setIvec3(glm::ivec3 v, std::string name)
{
    int loc = getUniformLocation(name.c_str());
    glProgramUniform3i(program, loc, v.x, v.y, v.z);
}

void Shader::setIvec4(glm::ivec4 v, std::string name)
{
    int loc = getUniformLocation(name.c_str());
    glProgramUniform4i(program, loc, v.x, v.y, v.z, v.w);
}

void Shader::setInt(int i, std::string name)
{
    int loc = getUniformLocation(name.c_str());
    glProgramUniform1i(program, loc, i);
}

void Shader::setFloat(float i, std::string name)
{
    int loc = getUniformLocation(name.c_str());
    glProgramUniform1f(program, loc, i);
}

void Shader::setFloatArray(std::vector<float> arr, std::string name)
{
    int loc = getUniformLocation(name.c_str());
    glProgramUniform1fv(program, loc, arr.size(), arr.data());
}

void Shader::setIntArray(std::vector<int>& arr, std::string name)
{
    int loc = getUniformLocation(name.c_str());
    glProgramUniform1iv(program, loc, arr.size(), arr.data());
}
