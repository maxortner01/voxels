#pragma once

#include "livre/header.h"
#include "livre/Graphics/RenderInstance.h"

namespace livre
{
    class DLLOUT Window
    {
        Graphics::RenderInstance* instance;
        void* window;

        std::string title;

    public:
        Window(uint16_t width, uint16_t height);
        ~Window();

        void clear() const;

        void setTitle(const std::string& title);

        const bool open() const;
        const void pollEvents() const;
        const void update() const;

        const Graphics::RenderInstance& getInstance() const;

        void waitForIdle() const;
    };
}