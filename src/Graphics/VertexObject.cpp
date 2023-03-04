#include "livre/livre.h"

namespace livre
{
    VertexObject::VertexObject(const wchar_t& buffertypes) :
        _count(0), buffers(nullptr)
    {
        glCreateVertexArrays(1, &id);

        const std::bitset<8> bits(buffertypes);

        for (int i = 0; i < 2; i++)
            if (bits[i]) 
                _count++;

        buffers = new BufferObject[_count];
        if (!buffers) { std::cout << "Insufficient memory malloc, vertexobject.\\n"; return; }

        bind();
        std::memset(buffers, 0, sizeof(BufferObject) * _count);
        
        int j = 0;
        for (int i = 0; i < 3; i++)
            if (bits[i])
            {
                buffers[j].setUp(j++, (BufferObject::BUFFERTYPE)pow(2, i));
            }
    }

    VertexObject::~VertexObject()
    {
        if (buffers)
        {
            std::free(buffers);
            buffers = nullptr;
        }

        glDeleteVertexArrays(1, &id);
    }

    bool VertexObject::hasBuffer(const BufferObject::BUFFERTYPE& type) const
    {
        for (int i = 0; i < _count; i++)
            if (buffers[i].getType() == type)
                return true;

        return false;
    }
    
    VertexObject::STATUS VertexObject::loadVertices(const Vec3f* vertices, const size_t& count) const
    {
        BufferObject* vertex_buffer = getBuffer(BufferObject::BUFFERTYPE::VERTEX);
        if (!vertex_buffer) return DOESNT_HAVE_BUFFER;
        
        bind();
        vertex_buffer->loadData((void*)vertices, sizeof(float), 3, count);
        unbind();

        return SUCCESS;
    }
    
    VertexObject::STATUS VertexObject::loadIndices(const uint32_t* indices, const size_t& count)
    {
        _indices = new uint32_t[count];
        if (!_indices) return ALLOCATION_FAILURE;

        if (!std::memcpy(_indices, indices, count * sizeof(uint32_t))) return COPY_FAILURE;

        _index_count = count;
        return SUCCESS;
    }

    void VertexObject::bind() const
    {
        glBindVertexArray(id);
    }

    void VertexObject::unbind() const
    {
        glBindVertexArray(0);
    }
    
    void VertexObject::render(const Shader& shader) const
    {
        bind();
        getBuffer(BufferObject::BUFFERTYPE::VERTEX)->bind();

        shader.use();

        glDrawElements(GL_LINES, _index_count, GL_UNSIGNED_INT, _indices);
        unbind();
    }

    uint32_t VertexObject::bufferCount() const
    {
        return _count;
    }
    
    BufferObject* VertexObject::getBuffer(const BufferObject::BUFFERTYPE& type) const
    {
        for (int i = 0; i < _count; i++)
            if (buffers[i].getType() == type)
                return &buffers[i];

        return nullptr;
    }

    BufferObject& VertexObject::operator[](const uint32_t& index) const
    {
        return buffers[index];
    }
}