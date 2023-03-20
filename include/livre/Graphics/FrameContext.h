#pragma once

#include "livre/header.h"

#include "Renderer.h"

namespace livre
{
    /**
     * @brief Wraps up renderer.startFrame() and .endFrame() calls into one scope.
     */
    class DLLOUT FrameContext
    {
        Renderer& _renderer;

    public:
        FrameContext(Renderer& renderer);
        ~FrameContext();
    };
}