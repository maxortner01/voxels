#pragma once

#include "BufferObject.h"

namespace livre
{ 
    class BufferArray
    {
        wchar_t _count;
        BufferObject* buffers;

    public:
        BufferArray(const wchar_t& type);
        ~BufferArray();

        BufferObject* operator[](const wchar_t& index) const;
        BufferObject* getBuffer(const wchar_t& index) const;
        BufferObject* getBuffer(const BufferObject::Type& type) const;
    };
}