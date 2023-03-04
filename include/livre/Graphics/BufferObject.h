#pragma once

#include "livre/header.h"


namespace livre
{
    class DLLOUT BufferObject
    {
    public:
        enum BUFFERTYPE
        {
            VERTEX = 1,
            MODELMAT = 2
        };

    private:
        uint32_t id;
        uint32_t _count;

        wchar_t _index;
        bool _databound;
        BUFFERTYPE _type;

    public:
        BufferObject();
        ~BufferObject();

        void setUp(const wchar_t& index, const BUFFERTYPE& type);
        void setIndex(const wchar_t& index);

        uint32_t getObjectCount() const;

        void bind() const;
        void unbind() const;

        uint32_t getID() const;
        BUFFERTYPE getType() const;

        void loadData(const void* data, const size_t& sizeof_element, const wchar_t& elements_per_object, const size_t& objects);
    };
}