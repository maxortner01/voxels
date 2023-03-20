#include "livre/livre.h"

namespace livre
{
    BufferArray::BufferArray(const wchar_t& type) :
        _count(0)
    {
#   ifdef LIVRE_LOGGING
        auto logger = spdlog::get("livre");
#   endif 

        const std::bitset<8> bits(type);

        for (wchar_t i = 0; i < 3; i++)
            if (bits[i]) _count++;

        buffers = new BufferObject[_count];
        if (!buffers) { ERROR_LOG("Error allocating buffers!"); return; }
        std::memset(buffers, 0, sizeof(BufferObject) * _count);

        wchar_t j = 0;
        for (wchar_t i = 0; i < 2; i++)
            if (bits[i])
                buffers[j++].setup((BufferObject::Type)(1 << i));
    }

    BufferArray::~BufferArray()
    {
        if (buffers)
        {
            delete[] buffers;
            buffers = nullptr;
        }
    }

    BufferObject* BufferArray::operator[](const wchar_t& index) const 
    { return getBuffer(index); }

    BufferObject* BufferArray::getBuffer(const wchar_t& index) const 
    {
        return &buffers[index];
    }

    BufferObject* BufferArray::getBuffer(const BufferObject::Type& type) const 
    {
        for (wchar_t i = 0; i < _count; i++)
            if (getBuffer(i)->getType() == type)
                return getBuffer(i);

        return nullptr;
    }
}