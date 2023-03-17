#pragma once

namespace livre
{
    class Renderer;
    class GraphicsPipeline;

namespace Graphics
{
    class Drawable
    {
    public:
        virtual void draw(const Renderer&, const GraphicsPipeline&) const = 0;
    };
}
}