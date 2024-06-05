#pragma once
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>

class Shader
{
public:
    Shader(const char* VertexShaderPath, const char* FragmentShaderPath, const char* GeometryShaderPath = nullptr)
    {
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        // Read Shader File
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        // ensure ifstream objects can throw exceptions:
        try
        {
            vShaderFile.open(VertexShaderPath);
            fShaderFile.open(FragmentShaderPath);
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            // if has geometry shader
            if (GeometryShaderPath != nullptr)
            {
                gShaderFile.open(GeometryShaderPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }
        catch(std::ifstream::failure& e)
        {
            std::cout << "[SHADER ERROR] FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }
        const char* VertexShaderSource = vertexCode.c_str();
        const char* FragmentShaderSource = fragmentCode.c_str();
        // Compile Shader and Shader Program
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(vertexShader, 1, &VertexShaderSource, nullptr);
        glCompileShader(vertexShader);
        CheckCompileErrors(vertexShader, "VERTEX");
        glShaderSource(fragmentShader, 1, &FragmentShaderSource, nullptr);
        glCompileShader(fragmentShader);
        CheckCompileErrors(vertexShader, "FRAGMENT");
        // if has geometry shader
        unsigned int geometryShader;
        if (GeometryShaderPath != nullptr)
        {
            const char * GeometryShaderSource = geometryCode.c_str();
            geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometryShader, 1, &GeometryShaderSource, NULL);
            glCompileShader(geometryShader);
            CheckCompileErrors(geometryShader, "GEOMETRY");
        }
        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);
        if (GeometryShaderPath != nullptr)
            glAttachShader(ID, geometryShader);
        glLinkProgram(ID);
        CheckCompileErrors(ID, "PROGRAM");
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        if (GeometryShaderPath != nullptr)
            glDeleteShader(geometryShader);
    }

    void Use()
    {
        glUseProgram(ID);
    }

    void DeleteShaderProgram()
    {
        glDeleteProgram(ID);
    }

    void SetInt(const char* name, const int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name), value);
    }

    void SetFloat(const char* name, const float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name), value);
    }

    void SetFloat(const std::string& name, const float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void SetMat4f(const std::string& name, const glm::mat4& value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, false, &value[0][0]);
    }

    void SetMat3f(const std::string& name, const glm::mat3& value) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, false, &value[0][0]);
    }

    void SetVec3f(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void SetVec3f(const std::string& name, const float x, const float y, const float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    // inline functions
    inline unsigned int GetID() const { return ID; }

private:
    // shader program
    unsigned int ID;

    void CheckCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if(type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};