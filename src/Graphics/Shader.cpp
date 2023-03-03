#include "livre/livre.h"

#include <fstream>
#include <cstring>
#include <bitset>
#include <cmath>

namespace livre
{
    Shader::Shader(const wchar_t& type) :
        _count(0), _id(0)
    {
        _id = glCreateProgram();
        if (!_id) return;

        const uint32_t GL_SHADERS[] = {
            GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_COMPUTE_SHADER
        };

        const std::bitset<8> bits(type);

        for (int i = 0; i < 3; i++)
            if (bits[i]) 
            {
                if (pow(2, i) == (int)TYPE::COMPUTE && _count > 0)
                {
                    std::cout << "Must not be any shaders along with a compute shader!\n";
                    return;
                }
                _count++;
            }

        files = (File*)std::malloc(sizeof(File) * _count);
        if (!files) { std::cout << "Insufficient memory malloc, shader.\\n"; return; }

        std::memset(files, 0, sizeof(File) * _count);
        int j = 0;
        for (int i = 0; i < 3; i++)
            if (bits[i])
            {
                files[j].type = (TYPE)pow(2, i);
                files[j++].id = glCreateShader(GL_SHADERS[i]);
            }
    }

    Shader::~Shader()
    {
        if (files)
        {
            std::free(files);
            files = nullptr;
        }
    }

    void Shader::use() const
    {
        glUseProgram(_id);
    }

    Shader::File Shader::getFile(const TYPE& type) const
    {
        for (int i = 0; i < _count; i++)
            if (files[i].type == type)
                return files[i];

        return { NONE, 0 };
    }

    Shader::STATUS Shader::fromFile(const TYPE& type, const std::string& filename) const 
    {
        if (!_id) return OPENGL_CREATESHADER_FAILED;

        Shader::File shaderFile = getFile(type);

        if (shaderFile.type == TYPE::NONE) 
            return STATUS::SHADER_NOT_MADE;
        else if (!shaderFile.id)
            return STATUS::OPENGL_CREATESHADER_FAILED;

        std::ifstream file(filename);
        if (!file) return STATUS::FILE_DOESNT_EXIST;

        std::string content( (std::istreambuf_iterator<char>(file) ),
                       (std::istreambuf_iterator<char>()    ) );

        return fromString(type, content);
    }

    Shader::STATUS Shader::fromString(const TYPE& type, const std::string& contents) const 
    {
        if (!_id) return OPENGL_CREATESHADER_FAILED;

        use();
        
        Shader::File shaderFile = getFile(type);

        const char* c_str = contents.c_str();
        int length        = contents.length();
        glShaderSource(shaderFile.id, 1, &c_str, &length);
        glCompileShader(shaderFile.id);

        int success;
        glGetShaderiv(shaderFile.id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(shaderFile.id, 512, nullptr, infoLog);

            std::cout << "Error compiling ";
            switch (shaderFile.type)
            {
            case TYPE::VERTEX:
                std::cout << "VERTEX";
                break;
                
            case TYPE::FRAGMENT:
                std::cout << "FRAGMENT";
                break;

            case TYPE::COMPUTE:
                std::cout << "COMPUTE";
                break;
            }

            std::cout << " shader!\n" << infoLog << "\n";

            return STATUS::SHADER_COMPILE_ERROR;
        }

        glAttachShader(_id, shaderFile.id);

        return STATUS::SUCCESS;
    }

    Shader::STATUS Shader::link() const
    {
        use();

        glLinkProgram(_id);

        int success;
        glGetProgramiv(_id, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(_id, 512, nullptr, infoLog);
            std::cout << "Error linking program!\n" << infoLog << "\n";

            return STATUS::PROGRAM_LINK_ERROR;
        }
        
        return STATUS::SUCCESS;
    }
}