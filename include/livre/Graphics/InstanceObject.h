#pragma once

#include "RenderInstance.h"

namespace livre
{
namespace Graphics
{
    /**
     * @brief Contains constant RenderInstance reference.
     * 
     * This is an easy way to extend the ability to reference a RenderInstance
     * object to various sub-classes
     */
    class InstanceObject
    {
    protected:
        const RenderInstance& _instance;

    public:
        InstanceObject(const RenderInstance& instance) :
            _instance(instance)
        {   }

        const RenderInstance& getInstance() const 
        { return _instance; }
    };
}
}