#ifndef SHADER_HPP
#define SHADER_HPP

#include "../lib/glad/include/glad/glad.h"
#include <string>

// taken from https://learnopengl.com

class Shader {
    private:
        unsigned int ID;
        void checkCompileErrors(unsigned int shader, std::string type);
    public:
        Shader();
        Shader(const char* vertexPath, const char* fragmentPath);
        unsigned int getID();
        void use();
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
        void setFloatMatrix4(const std::string &name, float* value) const;
};
#endif