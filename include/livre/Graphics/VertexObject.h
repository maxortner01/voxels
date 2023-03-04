#pragma once

#include "livre/header.h"

#include "BufferObject.h"
#include "Shader.h"

namespace livre
{
    class DLLOUT VertexObject 
    {
    public:
        enum STATUS
        {
            SUCCESS,
            DOESNT_HAVE_BUFFER,
            ALLOCATION_FAILURE,
            COPY_FAILURE
        };

    private:
        uint32_t id;
        uint32_t _count;
        BufferObject* buffers;
        
        size_t _index_count;
        uint32_t* _indices;

    public:
        VertexObject(const wchar_t& buffertypes);
        ~VertexObject();

        bool hasBuffer(const BufferObject::BUFFERTYPE& type) const;

        STATUS loadVertices(const Vec3f* vertices, const size_t& count) const;
        STATUS loadIndices(const uint32_t* indices, const size_t& count);

        void bind() const;
        void unbind() const;

        void render(const Shader& shader) const;

        uint32_t bufferCount() const;

        BufferObject* getBuffer(const BufferObject::BUFFERTYPE& type) const;
        BufferObject& operator[](const uint32_t& index) const;
    };
}