#pragma once

#include "livre/header.h"

namespace livre
{
    class BufferObject
    {
    public:
        enum Type
        {
            Vertex3D = (1 << 0),
            Vertex2D = (1 << 1),
            Color    = (1 << 2)
        };

        struct DataType
        {
            uint32_t size;
            wchar_t  element_count;
        };

    private:
        Type _type;

    public:
        ~BufferObject();

        void setup(const Type& type);

        Type getType() const;

        static DataType getDataType(const Type& type);
    };
} 
