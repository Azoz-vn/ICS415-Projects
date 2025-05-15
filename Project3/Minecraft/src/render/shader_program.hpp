#ifndef SHADER_PROGRAM_HPP
#define SHADER_PROGRAM_HPP

#include <glad/glad.h>

// STD
#include <string>
#include <unordered_map>
#include <iostream>

class ShaderProgram {
public:
    ShaderProgram();
    ~ShaderProgram();

    bool load(const std::string &vertexSource, const std::string &fragmentSource);
    void use() const;
    void destroy();

    // Uniform setters
    void setUniform(const std::string &name, int value);
    void setUniform(const std::string &name, float value);
    void setUniform(const std::string &name, float x, float y);
    void setUniform(const std::string &name, float x, float y, float z);
    void setUniform(const std::string &name, float x, float y, float z, float w);
    void setUniformMatrix4(const std::string &name, const float* matrix); // 4x4

    GLuint getProgramID() const { return programID; }

private:
    GLuint programID;
    std::unordered_map<std::string, GLint> uniformLocations;

    GLuint compileShader(GLenum type, const std::string &source);
    GLint getUniformLocation(const std::string &name);
};

#endif // SHADER_PROGRAM_HPP
