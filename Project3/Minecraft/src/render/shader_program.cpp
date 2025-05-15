#include "shader_program.hpp"

ShaderProgram::ShaderProgram() : programID(0) {}

ShaderProgram::~ShaderProgram() {
    destroy();
}

bool ShaderProgram::load(const std::string &vertexSource, const std::string &fragmentSource) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    if (!vertexShader || !fragmentShader)
        return false;

    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    GLint success = 0;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(programID, 512, nullptr, log);
        std::cerr << "Shader link error:\n" << log << std::endl;
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return true;
}

GLuint ShaderProgram::compileShader(GLenum type, const std::string &source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compile error:\n" << log << std::endl;
        return 0;
    }
    return shader;
}

void ShaderProgram::use() const {
    glUseProgram(programID);
}

void ShaderProgram::destroy() {
    if (programID != 0) {
        glDeleteProgram(programID);
        programID = 0;
    }
}

GLint ShaderProgram::getUniformLocation(const std::string &name) {
    if (uniformLocations.find(name) != uniformLocations.end())
        return uniformLocations[name];

    GLint loc = glGetUniformLocation(programID, name.c_str());
    uniformLocations[name] = loc;
    return loc;
}

void ShaderProgram::setUniform(const std::string &name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void ShaderProgram::setUniform(const std::string &name, float value) {
    glUniform1f(getUniformLocation(name), value);
}

void ShaderProgram::setUniform(const std::string &name, float x, float y) {
    glUniform2f(getUniformLocation(name), x, y);
}

void ShaderProgram::setUniform(const std::string &name, float x, float y, float z) {
    glUniform3f(getUniformLocation(name), x, y, z);
}

void ShaderProgram::setUniform(const std::string &name, float x, float y, float z, float w) {
    glUniform4f(getUniformLocation(name), x, y, z, w);
}

void ShaderProgram::setUniformMatrix4(const std::string &name, const float* matrix) {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, matrix);
}
