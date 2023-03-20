#include "livre/livre.h"

namespace livre
{
    FrameContext::FrameContext(Renderer& renderer) :
        _renderer(renderer)
    {
        _renderer.startFrame();
    }

    FrameContext::~FrameContext()
    {
        _renderer.endFrame();
    }
}