#pragma once

#include "livre/header.h"

#include "./Renderer.h"

namespace livre
{
    class DLLOUT ModelObject
    {
        friend class GraphicsPipeline;

    //private:
    //    void _recordCommandBuffer(const GraphicsPipeline& _pipeline, const uint32_t& imageIndex) const;

    public:
        //ModelObject();

        void draw(const Renderer& _renderer, const GraphicsPipeline& _pipeline) const;
    };
}