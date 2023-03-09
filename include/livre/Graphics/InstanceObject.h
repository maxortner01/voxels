#pragma once

#include "RenderInstance.h"

namespace livre
{
namespace Graphics
{
    class InstanceObject
    {
    protected:
        const RenderInstance& _instance;

    public:
        InstanceObject(const RenderInstance& instance) :
            _instance(instance)
        {   }
    };
}
}