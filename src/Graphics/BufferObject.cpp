#include "livre/livre.h"

namespace livre
{
    BufferObject::~BufferObject()
    {
        // delete
    }

    void BufferObject::setup(const BufferObject::Type& type)
    {
        _type  = type;
    }

    BufferObject::Type BufferObject::getType() const 
    { return _type; }

    BufferObject::DataType BufferObject::getDataType(const BufferObject::Type& type) 
    {
        DataType ret;

        if (type == Type::Vertex3D)
        {
            ret.size = sizeof(Vec3f);
            ret.element_count = 3;
        }
        else if (type == Type::Vertex2D)
        {
            ret.size = sizeof(Vec2f);
            ret.element_count = 2;
        }
        else if (type == Type::Color)
        {
            ret.size = sizeof(Color);
            ret.element_count = 4;
        }

        return ret;
    }
}