#pragma once

#include "livre/header.h"

namespace livre
{
    class DLLOUT Shader
    {
    public:
        enum STATUS
        {
            SUCCESS,
            OPENGL_CREATEPROGRAM_FAILED,
            SHADER_NOT_MADE,
            OPENGL_CREATESHADER_FAILED,
            FILE_DOESNT_EXIST,
            SHADER_COMPILE_ERROR,
            PROGRAM_LINK_ERROR,
            COMPILED,
            CREATED,
            DELETED
        };

        enum TYPE
        {
            NONE     = 0,
            VERTEX   = 1,
            FRAGMENT = 2,
            COMPUTE  = 4,
        };

        struct File
        {
            TYPE type;
            uint32_t id;
        };
    
    private:
        uint32_t _id;
        wchar_t _count;
        File* files;

    public:

        Shader(const wchar_t& type);
        ~Shader();
        
        void use() const;

        File getFile(const TYPE& type) const;

        STATUS fromFile(const TYPE& type, const std::string& filename) const;
        STATUS fromString(const TYPE& type, const std::string& contents) const;

        STATUS link() const;
    };
}