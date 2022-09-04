
// This is an independent project of an individual developer. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "Shader.h"


Shader::Shader()
{}
Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
   
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertex, fragment;
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

    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(this->program, 4096, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
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

void Shader::setInt(GLuint i, std::string name)
{
    int loc = getUniformLocation(name.c_str());
    glProgramUniform1i(program, loc, i);
}

void Shader::setFloat(GLuint i, std::string name)
{
    int loc = getUniformLocation(name.c_str());
    glProgramUniform1f(program, loc, i);
}