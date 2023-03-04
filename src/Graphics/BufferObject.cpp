#include "livre/livre.h"

namespace livre
{
    BufferObject::BufferObject() :
        _databound(false)
    {
    }
    
    void BufferObject::setUp(const wchar_t& index, const BUFFERTYPE& type)
    {
        glGenBuffers(1, &id);
        setIndex(index);
        _type = type;
    }

    void BufferObject::setIndex(const wchar_t& index)
    {
        _index = index;
    }

    uint32_t BufferObject::getObjectCount() const
    {
        return _count;
    }

    BufferObject::~BufferObject()
    {
        glDeleteBuffers(1, &id);
    }

    // These pure virtual functions crash on runtime
    // My suspicion is that VertexObject does not call the constructor,
    // so these functions are not build and therefore the undefined base versions
    // are what is left... maybe a redesign is necissary here. for now I'll stick
    // to calling GL explicitly
    void BufferObject::bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, id);
    }

    void BufferObject::unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    BufferObject::BUFFERTYPE BufferObject::getType() const
    {
        return _type;
    }

    void BufferObject::loadData(const void* data, const size_t& sizeof_element, const wchar_t& elements_per_object, const size_t& objects)
    {
        bind();

        //glBufferData(GL_ARRAY_BUFFER, size * count, data, (dynamic)?GL_DYNAMIC_DRAW:GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, elements_per_object * objects * sizeof_element, data, GL_STATIC_DRAW);

        unbind();

        _count = objects;

        if (_databound) return;
        
        bind();

        if (elements_per_object == 4 * 4)
        {            
            glEnableVertexAttribArray(_index + 0);
            glEnableVertexAttribArray(_index + 1);
            glEnableVertexAttribArray(_index + 2);
            glEnableVertexAttribArray(_index + 3);

            glVertexAttribPointer(_index + 0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (void*)(0));
            glVertexAttribPointer(_index + 1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (void*)(sizeof(float) * 4));
            glVertexAttribPointer(_index + 2, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (void*)(sizeof(float) * 8));
            glVertexAttribPointer(_index + 3, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 16, (void*)(sizeof(float) * 12));

            glVertexAttribDivisor(_index + 0, 1);
            glVertexAttribDivisor(_index + 1, 1);
            glVertexAttribDivisor(_index + 2, 1);
            glVertexAttribDivisor(_index + 3, 1);
        }
        else
        {
            glVertexAttribPointer(_index, elements_per_object, GL_FLOAT, false, 0, nullptr);
            glEnableVertexAttribArray(_index);
        }

        unbind();
        _databound = true;
    }
}