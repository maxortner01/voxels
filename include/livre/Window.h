#pragma once

#include "header.h"

namespace livre
{
    class DLLOUT Window
    {
        void* window;

    public:
        Window(uint16_t width, uint16_t height);

        const bool open() const;
        const void pollEvents() const;
        const void update() const;
    };
}