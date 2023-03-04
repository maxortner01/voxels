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
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("db_logger");
#   endif
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
#               ifdef LIVRE_LOGGING
                    logger->error("There can be no other shaders present with a compute shader!");
#               endif
                    return;
                }
                _count++;
            }

        files = (File*)std::malloc(sizeof(File) * _count);
#   ifndef LIVRE_LOGGING
        if (!files) return;
#   else
        if (!files) 
        { logger->error("Malloc failed: Insufficient memory!"); return; }
#   endif

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
        glDeleteProgram(_id);
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
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("db_logger");
#   endif
        Shader::File shaderFile = getFile(type);

#   ifndef LIVRE_LOGGING
        if (shaderFile.type == TYPE::NONE) 
            return STATUS::SHADER_NOT_MADE;
        else if (!shaderFile.id)
            return STATUS::OPENGL_CREATESHADER_FAILED;
#   else
        if (shaderFile.type == TYPE::NONE) 
            { logger->error("Shader (type {}) not present!", (int)(type)); return STATUS::SHADER_NOT_MADE; }
        else if (!shaderFile.id)
            { logger->error("OpenGL failed to create shader (type {})!", (int)(type)); return STATUS::OPENGL_CREATESHADER_FAILED; }
#   endif

        std::ifstream file(filename);

#   ifndef LIVRE_LOGGING
        if (!file) return STATUS::FILE_DOESNT_EXIST;
#   else
        if (!file) { logger->error("File '{}' does not exist!", filename); return STATUS::FILE_DOESNT_EXIST;}
#   endif

        std::string content( (std::istreambuf_iterator<char>(file) ),
                       (std::istreambuf_iterator<char>()    ) );

        return fromString(type, content);
    }

    Shader::STATUS Shader::fromString(const TYPE& type, const std::string& contents) const 
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("db_logger");
#   endif

#   ifndef LIVRE_LOGGING
        if (!_id) return OPENGL_CREATESHADER_FAILED;
#   else
        if (!_id) 
        { logger->error("OpenGL failed to create shader (type {})!", (int)type); return OPENGL_CREATESHADER_FAILED; }
#   endif

        use();
        
        Shader::File shaderFile = getFile(type);

        const char* c_str = contents.c_str();
        int length        = contents.length();
        glShaderSource(shaderFile.id, 1, &c_str, &length);
        glCompileShader(shaderFile.id);

        int success;
        glGetShaderiv(shaderFile.id, GL_COMPILE_STATUS, &success);
#   ifndef LIVRE_LOGGING
        if (!success) return STATUS::SHADER_COMPILE_ERROR;
#   else
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(shaderFile.id, 512, nullptr, infoLog);

            std::string content = "Error compiling ";

            switch (shaderFile.type)
            {
            case TYPE::VERTEX:
                content += "VERTEX";
                break;
                
            case TYPE::FRAGMENT:
                content += "FRAGMENT";
                break;

            case TYPE::COMPUTE:
                content += "COMPUTE";
                break;
            }

            content += " shader!\n";
            content += infoLog;

            logger->error(content);

            return STATUS::SHADER_COMPILE_ERROR;
        }
#   endif

        glAttachShader(_id, shaderFile.id);

#   ifdef LIVRE_LOGGING
        logger->info("Shader (type {}) compiled successfully!", (int)type);
#   endif
        return STATUS::SUCCESS;
    }

    Shader::STATUS Shader::link() const
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("db_logger");
#   endif

        use();

        glLinkProgram(_id);

        int success;
        glGetProgramiv(_id, GL_LINK_STATUS, &success);
#   ifndef LIVRE_LOGGING
        if (!success) return STATUS::PROGRAM_LINK_ERROR;
#   else
        if (!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(_id, 512, nullptr, infoLog);
            std::cout << "Error linking program!\n" << infoLog << "\n";

            logger->error("Error linking program {0}!\n{1}", _id, infoLog);
            return STATUS::PROGRAM_LINK_ERROR;
        }
#   endif
        
#   ifdef LIVRE_LOGGING
        logger->info("Program ({}) linked successfully!", _id);
#   endif
        return STATUS::SUCCESS;
    }
}